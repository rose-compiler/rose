// Include rose.h so we can manipulate ROSE objects to recognize and do the transformations
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rose.h>
#include "TransformationBase.h"
#define FILE_LEVEL_COPY_DEBUG_VALUE 0

SgStatement*
ROSE_TransformationBase::copy ( SgStatement *stmt, SgName oldName, SgName newName )
   {
  // error debugging variable
     int ROSE_COPY_DEBUG = FILE_LEVEL_COPY_DEBUG_VALUE;

     SgStatement* returnStatement = NULL;

     ROSE_ASSERT (stmt != NULL);
   
     char* notImplemented = "in COPY(STMT) NOT implemented";
     char* implemented = "in COPY(STMT) IS implemented";
   
     switch (stmt->variant())
        {
          case EXPR_STMT:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "EXPR_STMT " << implemented << endl;
               SgExprStatement* exprStmt = isSgExprStatement( stmt );
               ROSE_ASSERT (exprStmt != NULL);
               SgExpression *expr = exprStmt->get_the_expr();
               ROSE_ASSERT (expr != NULL);
               SgExpression* newExpression = copy(expr,oldName,newName);
               ROSE_ASSERT (newExpression != NULL);
               Sg_File_Info* fileInfo = buildSageFileInfo();

               SgExprStatement* newExprStatement = new SgExprStatement (fileInfo,newExpression);
               ROSE_ASSERT (newExprStatement != NULL);
#ifndef USE_SAGE3
	       newExprStatement->post_construction_initialization();
#endif

               ROSE_ASSERT (newExpression->get_parent() != NULL);

               returnStatement = newExprStatement;
               ROSE_ASSERT (returnStatement != NULL);
            // ROSE_ASSERT (returnStatement->get_parent() != NULL);
               break;
             }
          case FUNC_DECL_STMT:
               if (ROSE_COPY_DEBUG > 0) cout << "FUNC_DECL_STMT " << notImplemented << endl;
               ROSE_ASSERT (returnStatement != NULL);
               break;
          case MFUNC_DECL_STMT:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "MFUNC_DECL_STMT " << implemented << endl;

               SgMemberFunctionDeclaration* memberFunctionDeclarationStatement = isSgMemberFunctionDeclaration(stmt);
               ROSE_ASSERT (memberFunctionDeclarationStatement != NULL);

            // showSgStatement ( cout , memberFunctionDeclarationStatement, "Called from copy(SgStatement)" );

               SgName  oldName                             = memberFunctionDeclarationStatement->get_qualified_name();
               SgFunctionType* oldType                     = memberFunctionDeclarationStatement->get_type();
               SgFunctionDefinition* oldFunctionDefinition = memberFunctionDeclarationStatement->get_definition();
               SgClassDefinition* oldClassDefinition       = memberFunctionDeclarationStatement->get_scope();

            // printf ("oldName = %s \n",oldName.str());

               ROSE_ASSERT (oldType               != NULL);
	       ROSE_ASSERT (oldFunctionDefinition != NULL);
	       ROSE_ASSERT (oldClassDefinition    != NULL);

            // ROSE_ASSERT ( isSgClassDefinition(memberFunctionDeclarationStatement->get_parent()) != NULL );
            // ROSE_ASSERT (  (memberFunctionDeclarationStatement->get_scope()->get_declaration()->get_qualified_name()) != NULL);

            // tranform the name by hand since we are not building a SgInitializedName
            // object (maybe we need the string conversion at a lower level)
            // SgName newName                              = oldName;

            // Now we have to strip off the class name so that we can get the member function name
               const char* nameWithClass = oldName.str();

            // @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
            // Find the location of the "::" in the string "className::memberFunctionName"
	       char* startLocation = strpbrk (nameWithClass,":");
	       char* endLocation   = strpbrk (startLocation+1,":") + 1;  // add 1 to avoid the remaining ":" on the end;
            // printf ("startLocation = %s \n",startLocation);
            // printf ("endLocation   = %s \n",endLocation);
               ROSE_ASSERT (startLocation == endLocation-2);
               char* suffix = new char [128];
	       suffix = strcpy (suffix,endLocation);
	       SgName newName (suffix);
            // @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

            // printf ("newName = %s \n",newName.str());

               SgFunctionType* newType                     = (SgFunctionType*) copy (oldType,oldName,newName);
               SgFunctionDefinition* newFunctionDefinition = oldFunctionDefinition;

            // This defines the scope
	       SgClassDefinition* newClassDefinition       = oldClassDefinition;

               ROSE_ASSERT (newType               != NULL);
               ROSE_ASSERT (newFunctionDefinition != NULL);
               ROSE_ASSERT (newClassDefinition    != NULL);

               Sg_File_Info* fileInfo                      = buildSageFileInfo();

               SgMemberFunctionDeclaration* newMemberFunctionDeclarationStatement =
                    new SgMemberFunctionDeclaration(fileInfo,newName,newType,newFunctionDefinition,newClassDefinition);	       

            // The scope should be NULL after construction so we have to intialize the scope explicitly
               ROSE_ASSERT (newMemberFunctionDeclarationStatement->get_parent() == NULL);

            // reset the parent to the correct scope!
               if ( newMemberFunctionDeclarationStatement->get_parent() == NULL )
                  {
                    ROSE_ASSERT (getCurrentScope() != NULL);
                    newMemberFunctionDeclarationStatement->set_parent(getCurrentScope());
                  }

            // printf ("isSgClassDefinition(newMemberFunctionDeclarationStatement->get_parent()) = %s POINTER \n",
            //      (isSgClassDefinition(newMemberFunctionDeclarationStatement->get_parent()) == NULL) ? "NULL" : "VALID");

               ROSE_ASSERT (memberFunctionDeclarationStatement->get_parent() != NULL);
               if ( memberFunctionDeclarationStatement->get_parent() != NULL )
                    ROSE_ASSERT ( newMemberFunctionDeclarationStatement->get_parent() != NULL );
               else
                    ROSE_ASSERT ( newMemberFunctionDeclarationStatement->get_parent() == NULL );

               ROSE_ASSERT (memberFunctionDeclarationStatement->get_parent() != NULL);
               if ( isSgClassDefinition(memberFunctionDeclarationStatement->get_parent()) != NULL )
                    ROSE_ASSERT ( isSgClassDefinition(newMemberFunctionDeclarationStatement->get_parent()) != NULL );
               else
                    ROSE_ASSERT ( isSgClassDefinition(newMemberFunctionDeclarationStatement->get_parent()) == NULL );

               newMemberFunctionDeclarationStatement->set_parent(memberFunctionDeclarationStatement->get_parent());
               ROSE_ASSERT ( newMemberFunctionDeclarationStatement->get_parent() != NULL );

#if 0
               printf ("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! \n");
               printf ("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! \n");
               printf ("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! \n");
               printf ("Now unparse the new copy of the member function call Statement (memberFunctionDeclarationStatement) \n");
               memberFunctionDeclarationStatement->logical_unparse(NO_UNPARSE_INFO,cout);
               cout << endl << endl;
               printf ("Now unparse the new copy of the member function call Statement (newMemberFunctionDeclarationStatement) \n");
               newMemberFunctionDeclarationStatement->logical_unparse(NO_UNPARSE_INFO,cout);
               cout << endl << endl;
               printf ("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! \n");
               printf ("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! \n");
               printf ("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! \n");
#endif
               returnStatement = newMemberFunctionDeclarationStatement;
               ROSE_ASSERT (returnStatement != NULL);
               ROSE_ASSERT (returnStatement->get_parent() != NULL);
               break;
             }
          case FOR_STMT:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "FOR_STMT " << implemented << endl;

               SgForStatement* forStatement = isSgForStatement ( stmt );
	       ROSE_ASSERT (forStatement != NULL);

            // Copy the initialization statement
	       SgStatementPtrList & initializationStatementList = forStatement->get_init_stmt();

            // This causes a error:
            // "../../STL/function.h", line 27: Error: The operation "const list<SgStatement*> == const int" is illegal.
            // which is VERY difficult to track down so\ince no line number information is provided!!!
            // ROSE_ASSERT (initializationStatementList != NULL);

            // SgStatementPtrList* newInitializationStatement = copy (initializationStatementList,oldName,newName);
            // ROSE_ASSERT (newInitializationStatement != NULL);

            // Copy the conditional test
               SgExpression* testExpression = forStatement->get_test_expr();
	       ROSE_ASSERT (testExpression != NULL);
	       SgExpression* newTestExpression = copy (testExpression,oldName,newName);
	       ROSE_ASSERT (newTestExpression != NULL);

            // Copy the increment
               SgExpression* incrementExpression = forStatement->get_increment_expr();
	       ROSE_ASSERT (incrementExpression != NULL);
               SgExpression* newIncrementExpression = copy (incrementExpression,oldName,newName);
	       ROSE_ASSERT (newIncrementExpression != NULL);

            // Copy the loop body
               SgBasicBlock* loopBody = forStatement->get_loop_body();
	       ROSE_ASSERT (loopBody != NULL);
               SgStatement* newLoopBodyStatement = copy(loopBody,oldName,newName);
	       ROSE_ASSERT (newLoopBodyStatement != NULL);

            // Now we have to get the SgBasicBlock from the SgStatement (which is a SgBasciBlock in this case)
               SgBasicBlock* newLoopBodyBasicBlock = isSgBasicBlock(newLoopBodyStatement);
	       ROSE_ASSERT (newLoopBodyBasicBlock != NULL);

               Sg_File_Info* fileInfo = buildSageFileInfo();

               SgForStatement* newForStatement = new SgForStatement (fileInfo,newTestExpression,newIncrementExpression,newLoopBodyBasicBlock);
               ROSE_ASSERT (newForStatement != NULL);
               ROSE_ASSERT (newForStatement->get_parent() == NULL);

            // Now add the statements for the initialization statement list
            // to the new for loop's initialization statement list
               SgStatementPtrList::iterator statementIterator;
               for (statementIterator = initializationStatementList.begin();
                    statementIterator != initializationStatementList.end();
                    statementIterator++)
                  {
                 // Get the statement
#ifdef USE_SAGE3
                    SgStatement* initializationStatement = *statementIterator;
#else
                    SgStatement* initializationStatement = (*statementIterator).irep();
#endif
                    ROSE_ASSERT (initializationStatement != NULL);
                    ROSE_ASSERT (initializationStatement->get_parent() != NULL);
                    SgStatement* newInitializationStatement = copy (initializationStatement,oldName,newName);
                    ROSE_ASSERT (newInitializationStatement != NULL);
                    ROSE_ASSERT (newInitializationStatement->get_parent() == NULL);

                    ROSE_ASSERT (newForStatement != NULL);
                    newForStatement->append_init_stmt(newInitializationStatement);
                    ROSE_ASSERT (newInitializationStatement->get_parent() != NULL);
                  }

#if 0
               printf ("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! \n");
               printf ("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! \n");
               printf ("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! \n");
               printf ("Now unparse the new copy of the for Statement (newForStatement) \n");
               newForStatement->logical_unparse(NO_UNPARSE_INFO,cout);
               cout << endl << endl;
               printf ("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! \n");
               printf ("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! \n");
               printf ("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! \n");
#endif

               returnStatement = newForStatement;
               ROSE_ASSERT (returnStatement != NULL);
            // ROSE_ASSERT (returnStatement->get_parent() != NULL);
               break;
             }
          case DECL_STMT:
               if (ROSE_COPY_DEBUG > 0) cout << "DECL_STMT " << notImplemented << endl;
               ROSE_ASSERT (returnStatement != NULL);
               break;
          case SCOPE_STMT:
               if (ROSE_COPY_DEBUG > 0) cout << "SCOPE_STMT " << notImplemented << endl;
               ROSE_ASSERT (returnStatement != NULL);
               break;
          case FUNC_TBL_STMT:
               if (ROSE_COPY_DEBUG > 0) cout << "FUNC_TBL_STMT " << notImplemented << endl;
               ROSE_ASSERT (returnStatement != NULL);
               break;
          case GLOBAL_STMT:
               if (ROSE_COPY_DEBUG > 0) cout << "GLOBAL_STMT " << notImplemented << endl;
               ROSE_ASSERT (returnStatement != NULL);
               break;
          case BASIC_BLOCK_STMT:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "BASIC_BLOCK_STMT " << implemented << endl;
               SgBasicBlock* basicBlock = isSgBasicBlock(stmt);
	       ROSE_ASSERT (basicBlock != NULL);

               Sg_File_Info* fileInfo = buildSageFileInfo();

               SgBasicBlock* newBasicBlock = new SgBasicBlock(fileInfo);
	       ROSE_ASSERT (newBasicBlock != NULL);

               SgStatementPtrList::iterator statementIterator;
               for (statementIterator = basicBlock->get_statements().begin();
                    statementIterator != basicBlock->get_statements().end();
                    statementIterator++)
                  {
                 // Get each statement
#ifdef USE_SAGE3
                    SgStatement* currentStatement = *statementIterator;
#else
                    SgStatement* currentStatement = (*statementIterator).irep();
#endif
                    ROSE_ASSERT (currentStatement != NULL);

                 // Make a copy of the current statement
                    SgStatement* newCopyOfCurrentStatement = copy (currentStatement,oldName,newName);
                    ROSE_ASSERT (newCopyOfCurrentStatement != NULL);
                    ROSE_ASSERT (newCopyOfCurrentStatement->get_parent() == NULL);

                    ROSE_ASSERT (newBasicBlock != NULL);
                    newBasicBlock->append_statement(newCopyOfCurrentStatement);
                    ROSE_ASSERT (newCopyOfCurrentStatement->get_parent() != NULL);
                  }

               returnStatement = newBasicBlock;
               ROSE_ASSERT (returnStatement != NULL);
            // ROSE_ASSERT (returnStatement->get_parent() != NULL);
               break;
             }
          case IF_STMT:
               if (ROSE_COPY_DEBUG > 0) cout << "IF_STMT " << notImplemented << endl;
               ROSE_ASSERT (returnStatement != NULL);
               break;
          case FUNC_DEFN_STMT:
               if (ROSE_COPY_DEBUG > 0) cout << "FUNC_DEFN_STMT " << notImplemented << endl;
               ROSE_ASSERT (returnStatement != NULL);
               break;
		case VAR_DECL_STMT:
		{
			SgVariableDeclaration *variableDeclaration = isSgVariableDeclaration(stmt);
			ROSE_ASSERT (variableDeclaration != NULL);

			Sg_File_Info* fileInfo = buildSageFileInfo();

			SgVariableDeclaration* newVariableDeclaration = new SgVariableDeclaration (fileInfo);
			ROSE_ASSERT (newVariableDeclaration != NULL);

			SgInitializedNameList & initializedNameList = variableDeclaration->get_variables();
			SgInitializedNameList::iterator i;
			for (i = initializedNameList.begin(); i != initializedNameList.end(); i++)
			{
				// Now go though the list of initializedNames and copy them 
				// and add the copies to the newVariableDeclaration
				SgInitializedName& oldInitializedName = (*i);
				SgInitializedName* newInitializedName = copy (&oldInitializedName,oldName,newName);
				ROSE_ASSERT (newInitializedName != NULL);

#if 0
				cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
				cout << "Now unparse the old copy of the SgInitializedName Statement (oldInitializedName)" << endl;
				oldInitializedName.logical_unparse(NO_UNPARSE_INFO,cout);
				cout << endl << endl;
				cout << "Now unparse the new copy of the SgInitializedName Statement (newInitializedName)") << endl;
                newInitializedName.logical_unparse(NO_UNPARSE_INFO,cout);
				cout << endl << endl;
				cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
#endif


				ROSE_ASSERT(newInitializedName->get_named_item() != NULL);
				ROSE_ASSERT(newInitializedName->get_named_item()->get_initializer() == 
							oldInitializedName.get_named_item()->get_initializer());

				SgInitializer* oldInitializer = oldInitializedName.get_named_item()->get_initializer();
				SgInitializer* newInitializer = NULL;

				// CW: The initializer of a variable is optional so it might or might not exist
				// So, do a copy only if it exists. Otherwise use NULL as initializer for variable
				if (oldInitializer != NULL){
					newInitializer = copy (oldInitializer,oldName,newName);
					ROSE_ASSERT (newInitializer != NULL);
					ROSE_ASSERT (newInitializer->get_parent() == NULL);
#if 0
					cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
	                cout << "Now unparse the old copy of the SgInitializer Statement (oldInitializer)" << endl;
					oldInitializer->logical_unparse(NO_UNPARSE_INFO,cout);
					cout << endl << endl;
					cout << "Now unparse the new copy of the SgInitializer Statement (newInitializer)" << endl;
					newInitializer->logical_unparse(NO_UNPARSE_INFO,cout);
                    cout << endl << endl;
					cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
#endif			
				}

				newVariableDeclaration->append_variable(*newInitializedName,newInitializer);
			}
#if 0
			cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
			cout << "Now unparse the old copy of the SgVariableDeclaration Statement (variableDeclaration)" << endl;
			variableDeclaration->logical_unparse(NO_UNPARSE_INFO,cout);
			cout << endl << endl;
			cout << "Now unparse the new copy of the SgVariableDeclaration Statement (newVariableDeclaration)" << endl;
			newVariableDeclaration->logical_unparse(NO_UNPARSE_INFO,cout);
			cout << endl << endl;
			cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
#endif
			returnStatement = newVariableDeclaration;
			ROSE_ASSERT (returnStatement != NULL);
			break;
		}
          case VAR_DEFN_STMT:
               if (ROSE_COPY_DEBUG > 0) cout << "VAR_DEFN_STMT " << notImplemented << endl;
               ROSE_ASSERT (returnStatement != NULL);
               break;
          case CLASS_DECL_STMT:
               if (ROSE_COPY_DEBUG > 0) cout << "CLASS_DECL_STMT " << notImplemented << endl;
               ROSE_ASSERT (returnStatement != NULL);
               break;
          case CLASS_DEFN_STMT:
               if (ROSE_COPY_DEBUG > 0) cout << "CLASS_DEFN_STMT " << notImplemented << endl;
               ROSE_ASSERT (returnStatement != NULL);
               break;
          case ENUM_DECL_STMT:
               if (ROSE_COPY_DEBUG > 0) cout << "ENUM_DECL_STMT " << notImplemented << endl;
               ROSE_ASSERT (returnStatement != NULL);
               break;
          case LABEL_STMT:
               if (ROSE_COPY_DEBUG > 0) cout << "LABEL_STMT " << notImplemented << endl;
               ROSE_ASSERT (returnStatement != NULL);
               break;
          case WHILE_STMT:
               if (ROSE_COPY_DEBUG > 0) cout << "WHILE_STMT " << notImplemented << endl;
               ROSE_ASSERT (returnStatement != NULL);
               break;
          case DO_WHILE_STMT:
               if (ROSE_COPY_DEBUG > 0) cout << "DO_WHILE_STMT " << notImplemented << endl;
               ROSE_ASSERT (returnStatement != NULL);
               break;
          case SWITCH_STMT:
               if (ROSE_COPY_DEBUG > 0) cout << "SWITCH_STMT " << notImplemented << endl;
               ROSE_ASSERT (returnStatement != NULL);
               break;
          case CASE_STMT:
               if (ROSE_COPY_DEBUG > 0) cout << "CASE_STMT " << notImplemented << endl;
               ROSE_ASSERT (returnStatement != NULL);
               break;
          case TRY_STMT:
               if (ROSE_COPY_DEBUG > 0) cout << "TRY_STMT " << notImplemented << endl;
               ROSE_ASSERT (returnStatement != NULL);
               break;
          case CATCH_STMT:
               if (ROSE_COPY_DEBUG > 0) cout << "CATCH_STMT " << notImplemented << endl;
               ROSE_ASSERT (returnStatement != NULL);
               break;
          case DEFAULT_STMT:
               if (ROSE_COPY_DEBUG > 0) cout << "DEFAULT_STMT " << notImplemented << endl;
               ROSE_ASSERT (returnStatement != NULL);
               break;
          case BREAK_STMT:
               if (ROSE_COPY_DEBUG > 0) cout << "BREAK_STMT " << notImplemented << endl;
               ROSE_ASSERT (returnStatement != NULL);
               break;
          case CONTINUE_STMT:
               if (ROSE_COPY_DEBUG > 0) cout << "CONTINUE_STMT " << notImplemented << endl;
               ROSE_ASSERT (returnStatement != NULL);
               break;
          case RETURN_STMT:
               if (ROSE_COPY_DEBUG > 0) cout << "RETURN_STMT " << notImplemented << endl;
               ROSE_ASSERT (returnStatement != NULL);
               break;
          case GOTO_STMT:
               if (ROSE_COPY_DEBUG > 0) cout << "GOTO_STMT " << notImplemented << endl;
               ROSE_ASSERT (returnStatement != NULL);
               break;
          case ASM_STMT:
               if (ROSE_COPY_DEBUG > 0) cout << "ASM_STMT " << notImplemented << endl;
               ROSE_ASSERT (returnStatement != NULL);
               break;
          case SPAWN_STMT:
               if (ROSE_COPY_DEBUG > 0) cout << "SPAWN_STMT " << notImplemented << endl;
               ROSE_ASSERT (returnStatement != NULL);
               break;
          case PAR_STMT:
               if (ROSE_COPY_DEBUG > 0) cout << "PAR_STMT " << notImplemented << endl;
               ROSE_ASSERT (returnStatement != NULL);
               break;
          case PARFOR_STMT:
               if (ROSE_COPY_DEBUG > 0) cout << "PARFOR_STMT " << notImplemented << endl;
               ROSE_ASSERT (returnStatement != NULL);
               break;
          case TYPEDEF_STMT:
               if (ROSE_COPY_DEBUG > 0) cout << "TYPEDEF_STMT " << notImplemented << endl;
               ROSE_ASSERT (returnStatement != NULL);
               break;
          case TEMPLATE_DECL_STMT:
               if (ROSE_COPY_DEBUG > 0) cout << "TEMPLATE_DECL_STMT " << notImplemented << endl;
               ROSE_ASSERT (returnStatement != NULL);
               break;
#if USE_SAGE3
          case PRAGMA_STMT:
             {
            // if (ROSE_COPY_DEBUG > 0) cout << "PRAGMA_STMT " << notImplemented << endl;
            // break;

               if (ROSE_COPY_DEBUG > 0) cout << "PRAGMA_STMT " << implemented << endl;
               SgPragmaStatement* pragmaStmt = isSgPragmaStatement( stmt );
               ROSE_ASSERT (pragmaStmt != NULL);
               SgPragma *pragma = pragmaStmt->get_pragma();
               ROSE_ASSERT (pragma != NULL);

            // Don't do editing of the SgPragma object!
               SgPragma* newPragma = copy(pragma);
               ROSE_ASSERT (newPragma != NULL);
               Sg_File_Info* fileInfo = buildSageFileInfo();

               SgPragmaStatement* newPragmaStatement = new SgPragmaStatement (fileInfo,newPragma);
               ROSE_ASSERT (newPragmaStatement != NULL);

	       ROSE_ASSERT (newPragmaStatement->get_parent() == NULL);

               returnStatement = newPragmaStatement;
               ROSE_ASSERT (returnStatement != NULL);
            // ROSE_ASSERT (returnStatement->get_parent() != NULL);
               break;
             }
#endif
          default:
               cout << "copy.C, copy(SgStatement) -- default reached" << endl;
               ROSE_ABORT();
        }

     ROSE_ASSERT (returnStatement != NULL);
  // ROSE_ASSERT (returnStatement->get_parent() != NULL);

     return returnStatement;
   }

#if USE_SAGE3
SgPragma*
ROSE_TransformationBase::copy ( SgPragma *pragma )
   {
  // Copy the existing pragma but skip the name editing (on the pragma's text string)
     ROSE_ASSERT(pragma != NULL);
     SgPragma* returnPragma = NULL;

     char* pragmaText    = pragma->get_name();
     ROSE_ASSERT(pragmaText != NULL);

  // Copy the string to avoid strange dependencies on strings within ROSE
     char* newPragmaText = strdup(pragmaText);
     ROSE_ASSERT(newPragmaText != NULL);

     Sg_File_Info* fileInfo = buildSageFileInfo();

     returnPragma = new SgPragma(newPragmaText,fileInfo);

     ROSE_ASSERT(returnPragma != NULL);
     return returnPragma;
   }
#endif


// The SgStatementPtrList objects are not often referenced through pointers
SgStatementPtrList* 
ROSE_TransformationBase::copy ( SgStatementPtrList & statementList, SgName oldName, SgName newName )
   {
     SgStatementPtrList* returnStatementList = new SgStatementPtrList;
     ROSE_ASSERT (returnStatementList != NULL);

     printf ("Not a corretly implemented copy function for SgStatementPtrList \n");
     ROSE_ABORT();

#if 0
     SgStatementPtrList::iterator statementIterator;
     for (statementIterator = newLoopBodyBasicBlock->get_statements().begin();
          statementIterator != newLoopBodyBasicBlock->get_statements().end();
          statementIterator++)
        {
       // Get the statement
          SgStatement* sageStatement = (*statementIterator).irep();

       // Get the SgStatement from the SgStatementPrt using the irep() member function
          ROSE_Statement* roseStatement = copy ( sageStatement );
          ROSE_ASSERT (roseStatement != NULL);

       // Now put the new statement into the list of statements stored in the statement block
          ROSE_ASSERT (returnStatementList != NULL);
          returnStatementList->addArrayStatementToBlock(roseStatement);
        }
#endif

     ROSE_ASSERT (returnStatementList != NULL);
     return returnStatementList;
   }

#if 0
SgBinaryOp*
ROSE_TransformationBase::copy ( SgBinaryOp *binaryOp, SgName oldName, SgName newName )
   {
     SgBinaryOp* returnBinaryOp = NULL;

     printf ("Sorry, not implemented: Exiting within ROSE_TransformationBase::copy ( SgBinaryOp *binaryOp ) \n");
     ROSE_ABORT();

     if (binaryOp == NULL)
        {
          printf ("copy(SgBinaryOP==NULL) \n");
          return returnBinaryOp;
        }

     ROSE_ASSERT (binaryOp != NULL);

     cout << "BINARY_EXPRESSION implemented" << endl;
     ROSE_ASSERT (isSgBinaryOp(binaryOp) != NULL);

     copy(binaryOp->get_lhs_operand(),oldName,newName,TRUE);
     copy(binaryOp->get_rhs_operand(),oldName,newName,FALSE);

     SgType* type = binaryOp->get_type();
     copy(type,oldName,newName);

     ROSE_ASSERT (returnBinaryOp != NULL);
     return returnBinaryOp;
   }
#endif

SgInitializer*
ROSE_TransformationBase::copy ( SgInitializer *expr, SgName oldName, SgName newName )
   {
     SgInitializer* returnInitializer = NULL;

     ROSE_ASSERT (expr != NULL);

  // error debugging variable
     int ROSE_COPY_DEBUG = FILE_LEVEL_COPY_DEBUG_VALUE;

     char* notImplemented = "in COPY(SgInitializer) NOT implemented";
     char* implemented    = "in COPY(SgInitializer) IS implemented";

  // printf ("Inside of ROSE_TransformationBase::copy ( SgInitializer *expr, char* oldName, char* newName ) \n");

#if 0
     printf ("\n");
     printf ("################################################# \n");
     printf ("AT TOP OF COPY OPERATION the input expression is: \n");
     showSgExpression(cout,expr,"ROSE_TransformationBase::copy(SgExpression)");
     printf ("################################################# \n");
     printf ("\n");
#endif

     switch (expr->variant())
        {
          case EXPR_INIT:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "EXPR_INIT " << notImplemented << endl;
               cout << "EXPR_INIT not implemented" << endl;
               SgInitializer* initializer = isSgInitializer(expr);
               ROSE_ASSERT (initializer != NULL);
#if 0
               SgInitializer* newInitializer = new SgInitializer(expr);
               ROSE_ASSERT (newInitializer);

	       returnInitializer = newInitializer;
#endif
               ROSE_ASSERT (returnInitializer != NULL);
               ROSE_ASSERT (returnInitializer->get_parent() != NULL);
               break;
             }
          case AGGREGATE_INIT:
               if (ROSE_COPY_DEBUG > 0) cout << "AGGREGATE_INIT " << notImplemented << endl;
               ROSE_ASSERT (returnInitializer != NULL);
               ROSE_ASSERT (returnInitializer->get_parent() != NULL);
               break;
          case CONSTRUCTOR_INIT:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "CONSTRUCTOR_INIT " << implemented << endl;
               SgConstructorInitializer* constructorInitializer = isSgConstructorInitializer( expr );
               ROSE_ASSERT (constructorInitializer != NULL);

               Sg_File_Info* fileInfo = buildSageFileInfo();

               SgMemberFunctionDeclaration *memberFunctionDeclaration = constructorInitializer->get_declaration();
            // We don't need the member function declaration if it is not available
            // ROSE_ASSERT (memberFunctionDeclaration != NULL);

               SgExprListExp* exprListExp = constructorInitializer->get_args();
               ROSE_ASSERT (exprListExp != NULL);

               SgExprListExp* newExprListExp = (SgExprListExp*) copy(exprListExp,oldName,newName);
               ROSE_ASSERT (newExprListExp != NULL);

               SgClassDeclaration* classDeclaration = constructorInitializer->get_class_decl();
            // We don't need the class declaration if it is not available (but it should always be available???)
            // ROSE_ASSERT (classDeclaration != NULL);

            // Not clear what this does!
               int needParenthesis = constructorInitializer->get_need_paren();
            // needParenthesis = FALSE;

               SgConstructorInitializer* newConstructorInitializer = 
                    new SgConstructorInitializer (fileInfo, memberFunctionDeclaration, newExprListExp, classDeclaration, needParenthesis);
               ROSE_ASSERT (newConstructorInitializer != NULL);

               returnInitializer = newConstructorInitializer;
               ROSE_ASSERT (returnInitializer != NULL);
               ROSE_ASSERT (returnInitializer->get_parent() != NULL);
               break;
             }
          case ASSIGN_INIT:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "ASSIGN_INIT " << implemented << endl;
               SgAssignInitializer* assignInitializer = isSgAssignInitializer(expr);
               ROSE_ASSERT (assignInitializer != NULL);

               Sg_File_Info* fileInfo = buildSageFileInfo();

               SgExpression* expression = assignInitializer->get_operand();
               ROSE_ASSERT (expression != NULL);
	       SgExpression* newExpression = copy(expression,oldName,newName);
	       ROSE_ASSERT (newExpression != NULL);
               ROSE_ASSERT (newExpression->get_parent() == NULL);


               newExpression->set_parent(expression->get_parent());

#if 0
               printf ("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! \n");
               printf ("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! \n");
               printf ("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! \n");
               printf ("Now unparse the old copy of the SgExpression (expression) \n");
               expression->logical_unparse(NO_UNPARSE_INFO,cout);
               cout << endl << endl;
	       showSgExpression ( cout, expression, "Called from copy(SgAssignInitializer*) (expression)" );

               printf ("Now unparse the new copy of the SgExpression (newExpression) \n");
               newExpression->logical_unparse(NO_UNPARSE_INFO,cout);
               cout << endl << endl;
	       showSgExpression ( cout, newExpression, "Called from copy(SgAssignInitializer*) (newExpression)" );
               printf ("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! \n");
               printf ("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! \n");
               printf ("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! \n");
#endif

               SgType* type = assignInitializer->get_type();
	       ROSE_ASSERT (type);
               SgType* newType = copy (type,oldName,newName);
	       ROSE_ASSERT (newType != NULL);

               sage_bool needParenthesis = assignInitializer->get_need_paren();

               SgInitializer* newAssignInitializer = new SgAssignInitializer(fileInfo,newExpression,newType,needParenthesis);
               ROSE_ASSERT (newAssignInitializer);

            // ROSE_ASSERT (newExpression->get_parent() == NULL);
            // newExpression->set_parent(newAssignInitializer);
               ROSE_ASSERT (newExpression->get_parent() != NULL);

	       returnInitializer = newAssignInitializer;
               ROSE_ASSERT (returnInitializer != NULL);
               ROSE_ASSERT (returnInitializer->get_parent() == NULL);
            // ROSE_ASSERT (returnInitializer->get_parent() != NULL);
               break;
             }
          default:
               cerr << "copy.C, copy(SgInitializer*) -- default reached" << endl;
        }

     ROSE_ASSERT (returnInitializer != NULL);
  // ROSE_ASSERT (returnInitializer->get_parent() != NULL);
     return returnInitializer;
   }

#define VALUE_EXPRESSION_MACRO(OPERATION_CLASSNAME,OPERATION_CLASSNAME_TEST,OLD_VARIABLENAME,NEW_VARIABLENAME) \
               OPERATION_CLASSNAME* OLD_VARIABLENAME = OPERATION_CLASSNAME_TEST(expr); \
	       ROSE_ASSERT (OLD_VARIABLENAME != NULL);                                 \
               Sg_File_Info* fileInfo = buildSageFileInfo();                           \
               OPERATION_CLASSNAME* NEW_VARIABLENAME =                                 \
                    new OPERATION_CLASSNAME (fileInfo,OLD_VARIABLENAME->get_value());  \
               returnExpression = NEW_VARIABLENAME;                                    \
	       ROSE_ASSERT (returnExpression!= NULL);

#define UNARY_OPERATOR_MACRO(OPERATION_CLASSNAME,OPERATION_CLASSNAME_TEST,OLD_VARIABLENAME,NEW_VARIABLENAME) \
               OPERATION_CLASSNAME * OLD_VARIABLENAME = OPERATION_CLASSNAME_TEST (expr);          \
               ROSE_ASSERT (OLD_VARIABLENAME != NULL);                                            \
               Sg_File_Info* fileInfo  = buildSageFileInfo();                                     \
               SgExpression* unaryExpr = copy(OLD_VARIABLENAME->get_operand(),oldName,newName);   \
               SgType* exprType        = copy(OLD_VARIABLENAME->get_type(),oldName,newName);      \
               ROSE_ASSERT (unaryExpr != NULL);                                                   \
               ROSE_ASSERT (exprType  != NULL);                                                    \
               OPERATION_CLASSNAME* NEW_VARIABLENAME =                                            \
                    new OPERATION_CLASSNAME (fileInfo,unaryExpr,exprType);                        \
               ROSE_ASSERT (NEW_VARIABLENAME != NULL);                                            \
               ROSE_ASSERT (unaryExpr->get_parent() != NULL);                                     \
               ROSE_ASSERT (OLD_VARIABLENAME->get_operand() != NULL);                             \
               ROSE_ASSERT (OLD_VARIABLENAME->get_operand()->get_parent() != NULL);               \
               ROSE_ASSERT (unaryExpr->get_parent()->variant() == OLD_VARIABLENAME->get_operand()->get_parent()->variant()); \
	       returnExpression = NEW_VARIABLENAME;                                               \
               ROSE_ASSERT (returnExpression != NULL);

#define BINARY_OPERATOR_MACRO(OPERATION_CLASSNAME,OPERATION_CLASSNAME_TEST,OLD_VARIABLENAME,NEW_VARIABLENAME) \
               OPERATION_CLASSNAME * OLD_VARIABLENAME = OPERATION_CLASSNAME_TEST (expr);          \
               ROSE_ASSERT (OLD_VARIABLENAME != NULL);                                            \
               Sg_File_Info* fileInfo = buildSageFileInfo();                                      \
               SgExpression* lhsExpr  = copy(OLD_VARIABLENAME->get_lhs_operand(),oldName,newName); \
               SgExpression* rhsExpr  = copy(OLD_VARIABLENAME->get_rhs_operand(),oldName,newName); \
               SgType* exprType       = copy(OLD_VARIABLENAME->get_type(),oldName,newName);        \
               ROSE_ASSERT (lhsExpr  != NULL);                                                     \
               ROSE_ASSERT (rhsExpr  != NULL);                                                     \
               ROSE_ASSERT (exprType != NULL);                                                    \
               OPERATION_CLASSNAME* NEW_VARIABLENAME =                                            \
                    new OPERATION_CLASSNAME (fileInfo,lhsExpr,rhsExpr,exprType);                  \
               ROSE_ASSERT (NEW_VARIABLENAME != NULL);                                            \
               ROSE_ASSERT (lhsExpr->get_parent() != NULL);                                       \
               ROSE_ASSERT (rhsExpr->get_parent() != NULL);                                       \
               ROSE_ASSERT (OLD_VARIABLENAME->get_lhs_operand() != NULL);                         \
               ROSE_ASSERT (OLD_VARIABLENAME->get_rhs_operand() != NULL);                         \
               ROSE_ASSERT (OLD_VARIABLENAME->get_lhs_operand()->get_parent() != NULL);           \
               ROSE_ASSERT (OLD_VARIABLENAME->get_rhs_operand()->get_parent() != NULL);           \
               ROSE_ASSERT (lhsExpr->get_parent()->variant() == OLD_VARIABLENAME->get_lhs_operand()->get_parent()->variant()); \
               ROSE_ASSERT (rhsExpr->get_parent()->variant() == OLD_VARIABLENAME->get_rhs_operand()->get_parent()->variant()); \
	       returnExpression = NEW_VARIABLENAME;                                               \
               ROSE_ASSERT (returnExpression != NULL);


SgExpression*
ROSE_TransformationBase::copy ( SgExpression *expr, SgName oldName, SgName newName )
   {
     SgExpression* returnExpression = NULL;

     ROSE_ASSERT (expr != NULL);

  // error debugging variable
     int ROSE_COPY_DEBUG = FILE_LEVEL_COPY_DEBUG_VALUE;

     char* notImplemented = "in COPY(SgExpression) NOT implemented";
     char* implemented    = "in COPY(SgExpression) IS implemented";

#if 0
     printf ("\n");
     printf ("################################################# \n");
     printf ("AT TOP OF COPY OPERATION the input expression is: \n");
     showSgExpression(cout,expr,"ROSE_TransformationBase::copy(SgExpression)");
     printf ("################################################# \n");
     printf ("\n");
#endif

     switch (expr->variant())
        {
          case UNARY_EXPRESSION:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "UNARY_EXPRESSION " << implemented << endl;
               ROSE_ABORT();
               ROSE_ASSERT (returnExpression != NULL);
               break;
             }
          case BINARY_EXPRESSION:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "BINARY_EXPRESSION " << implemented << endl;
               ROSE_ABORT();
               ROSE_ASSERT (returnExpression != NULL);
               break;
             }
#ifndef USE_SAGE3
          case ASSIGNMENT_EXPRESSION:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "ASSIGNMENT_EXPRESSION " << implemented << endl;
               ROSE_ASSERT (returnExpression != NULL);
               break;
             }
#endif
          case EXPRESSION_ROOT:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "EXPRESSION_ROOT " << implemented << endl;
               ROSE_ABORT();
#if 1
               UNARY_OPERATOR_MACRO (SgExpressionRoot,isSgExpressionRoot,expressionRoot,newExpressionRoot)
#else
               SgExpressionRoot *expressionRoot = isSgExpressionRoot(expr);
               ROSE_ASSERT (expressionRoot != NULL);
               SgStatement* parentStatement = expressionRoot->get_statement();
               ROSE_ASSERT (parentStatement != NULL);
            // copy(parentStatement);
               returnExpression = NULL;
               ROSE_ASSERT (returnExpression != NULL);
               ROSE_ASSERT (returnExpression->get_parent() != NULL);
#endif
               break;
             }
          case EXPR_LIST:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "EXPR_LIST " << implemented << endl;
               SgExprListExp* exprListExp = isSgExprListExp( expr );
               ROSE_ASSERT (exprListExp != NULL);

               Sg_File_Info* fileInfo = buildSageFileInfo();

               SgExprListExp* newExpressionListExpression = new SgExprListExp (fileInfo);
               ROSE_ASSERT (newExpressionListExpression != NULL);

               SgExpressionPtrList & expressionPtrList = exprListExp->get_expressions();
               SgExpressionPtrList::iterator i = expressionPtrList.begin();
               if (i == expressionPtrList.end())
                  {
                    if ( (ROSE_DEBUG > 0) || (ROSE::verbose == TRUE) )
                         cout << "in copy(SgExprListExp*) found an <EMPTY LIST>" << endl;
                  }
                 else
                  {
                    while (i != expressionPtrList.end())
                       {
#ifdef USE_SAGE3
                         SgExpression* oldExpressionListElement = *i;
#else
                         SgExpressionPtr expressionPtr = *i;
                         SgExpression* oldExpressionListElement = expressionPtr.operator->();
#endif
                         ROSE_ASSERT (oldExpressionListElement != NULL);
                      // showSgExpression( os, expressionPtr.operator->(), depth+1 );

                         SgExpression* newExpressionListElement = copy (oldExpressionListElement,oldName,newName);
                         ROSE_ASSERT (newExpressionListElement != NULL);

                         ROSE_ASSERT (newExpressionListExpression != NULL);
                         newExpressionListExpression->append_expression(newExpressionListElement);
                         i++;
                       }
                  }

	       returnExpression = newExpressionListExpression;
               ROSE_ASSERT (returnExpression != NULL);
               ROSE_ASSERT (returnExpression->get_parent() == NULL);
            // ROSE_ASSERT (returnExpression->get_parent() != NULL);
               break;
	     }
          case VAR_REF:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "VAR_REF " << implemented << endl;
               SgVarRefExp* varRefExp = isSgVarRefExp( expr );
	       ROSE_ASSERT (varRefExp != NULL);

               Sg_File_Info* fileInfo = buildSageFileInfo();

               SgVariableSymbol* variableSymbol = varRefExp->get_symbol();
	       ROSE_ASSERT (variableSymbol != NULL);

               SgVariableSymbol* newVariableSymbol = (SgVariableSymbol*) copy (variableSymbol,oldName,newName);
	       ROSE_ASSERT (newVariableSymbol != NULL);

               SgVarRefExp* newVarRefExpression = new SgVarRefExp (fileInfo,newVariableSymbol);
	       ROSE_ASSERT (newVarRefExpression != NULL);

            // showSgExpression ( cout, newVarRefExpression, "Called from copy(VAR_REF)" );

	       returnExpression = newVarRefExpression;
            // printf ("EXITING: CHECK COPY in case of VAR_REF! \n");
            // ROSE_ABORT();
               ROSE_ASSERT (returnExpression != NULL);
            // ROSE_ASSERT (returnExpression->get_parent() != NULL);
               break;
             }
          case CLASSNAME_REF:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "CLASSNAME_REF " << implemented << endl;
               ROSE_ASSERT (returnExpression != NULL);
               break;
             }
          case FUNCTION_REF:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "FUNCTION_REF " << implemented << endl;
               SgFunctionRefExp* functionRefExp = isSgFunctionRefExp( expr );
               ROSE_ASSERT (functionRefExp != NULL);
      
               Sg_File_Info* fileInfo = buildSageFileInfo();

               SgFunctionSymbol* functionSymbol = functionRefExp->get_symbol();
               ROSE_ASSERT (functionSymbol != NULL);

               SgFunctionType* functionType = functionRefExp->get_function_type();
               ROSE_ASSERT (functionType != NULL);

               SgFunctionType* newFunctionType = (SgFunctionType*) copy (functionType,oldName,newName);
               ROSE_ASSERT (newFunctionType != NULL);

               SgFunctionSymbol* newFunctionSymbol = (SgFunctionSymbol*) copy (functionSymbol,oldName,newName);
               ROSE_ASSERT (newFunctionSymbol != NULL);

            // Use the existing memberFunctionSymbol and the existing functionType
               SgFunctionRefExp* newFunctionRefExp =
                    new SgFunctionRefExp (fileInfo, newFunctionSymbol, newFunctionType);
	       ROSE_ASSERT (newFunctionRefExp != NULL);

	       returnExpression = newFunctionRefExp;
               ROSE_ASSERT (returnExpression != NULL);
               break;
             }
          case MEMBER_FUNCTION_REF:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "MEMBER_FUNCTION_REF " << implemented << endl;

               SgMemberFunctionRefExp* memberFunctionRefExp = isSgMemberFunctionRefExp( expr );
               ROSE_ASSERT (memberFunctionRefExp != NULL);
      
               Sg_File_Info* fileInfo = buildSageFileInfo();

            // We assume this for now (not clear if it makes much difference for ROSE)
               Boolean isVirtualFunction = FALSE;  

               SgMemberFunctionSymbol* memberFunctionSymbol = memberFunctionRefExp->get_symbol();
               ROSE_ASSERT (memberFunctionSymbol != NULL);

               SgFunctionType* functionType = memberFunctionRefExp->get_function_type();
               ROSE_ASSERT (functionType != NULL);

               SgFunctionType* newFunctionType = (SgFunctionType*) copy (functionType,oldName,newName);
               ROSE_ASSERT (newFunctionType != NULL);

               SgMemberFunctionSymbol* newMemberFunctionSymbol = (SgMemberFunctionSymbol*) copy (memberFunctionSymbol,oldName,newName);
               ROSE_ASSERT (newMemberFunctionSymbol != NULL);

            // Use the existing memberFunctionSymbol and the existing functionType
               SgMemberFunctionRefExp* newMemberFunctionRefExp =
                    new SgMemberFunctionRefExp (fileInfo, newMemberFunctionSymbol, isVirtualFunction, newFunctionType);
	       ROSE_ASSERT (newMemberFunctionRefExp != NULL);

            // This controls the unparsing of the name and if it uses the "xxx::" in front of the function call
               newMemberFunctionRefExp->set_need_qualifier(memberFunctionRefExp->get_need_qualifier());
               ROSE_ASSERT (newMemberFunctionRefExp->get_need_qualifier() == memberFunctionRefExp->get_need_qualifier());

#if 0
               printf ("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! \n");
               printf ("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! \n");
               printf ("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! \n");
               printf ("Now unparse the old copy of the SgExpression (functionExpression) \n");
               memberFunctionSymbol->logical_unparse(NO_UNPARSE_INFO,cout);
               cout << endl << endl;
	       showSgSymbol ( cout, memberFunctionSymbol, "Called from copy(SgMemberFunctionRefExp*) (memberFunctionSymbol)" );

               printf ("Now unparse the new copy of the SgExpression (newFunctionExpression) \n");
               newMemberFunctionSymbol->logical_unparse(NO_UNPARSE_INFO,cout);
               cout << endl << endl;
	       showSgSymbol ( cout, newMemberFunctionSymbol, "Called from copy(SgMemberFunctionRefExp*) (newMemberFunctionSymbol)" );
               printf ("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! \n");
               printf ("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! \n");
               printf ("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! \n");
#endif

	       returnExpression = newMemberFunctionRefExp;
               ROSE_ASSERT (returnExpression != NULL);
            // ROSE_ASSERT (returnExpression->get_parent() != NULL);
               break;
             }
          case BOOL_VAL:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "BOOL_VAL " << notImplemented << endl;
               VALUE_EXPRESSION_MACRO (SgBoolValExp,isSgBoolValExp,boolValExp,newBoolValExp)
               break;
             }
          case SHORT_VAL:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "SHORT_VAL " << notImplemented << endl;
               VALUE_EXPRESSION_MACRO (SgShortVal,isSgShortVal,shortVal,newShortVal)
               break;
             }
          case CHAR_VAL:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "CHAR_VAL " << notImplemented << endl;
               VALUE_EXPRESSION_MACRO (SgCharVal,isSgCharVal,charVal,newCharVal)
               break;
             }
          case UNSIGNED_CHAR_VAL:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "UNSIGNED_CHAR_VAL " << notImplemented << endl;
               VALUE_EXPRESSION_MACRO (SgUnsignedCharVal,isSgUnsignedCharVal,unsignedCharVal,newUnsignedCharVal)
               break;
             }
          case WCHAR_VAL:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "WCHAR_VAL " << notImplemented << endl;
               VALUE_EXPRESSION_MACRO (SgWcharVal,isSgWcharVal,wcharVal,newWcharVal)
               break;
             }
          case STRING_VAL:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "STRING_VAL " << notImplemented << endl;
               VALUE_EXPRESSION_MACRO (SgStringVal,isSgStringVal,stringVal,newStringVal)
               break;
             }
          case UNSIGNED_SHORT_VAL:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "UNSIGNED_SHORT_VAL " << notImplemented << endl;
               VALUE_EXPRESSION_MACRO (SgUnsignedShortVal,isSgUnsignedShortVal,unsignedShortVal,newUnsignedShortVal)
               break;
             }
          case ENUM_VAL:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "ENUM_VAL " << notImplemented << endl;
            // This case is not implemented yet!
            // VALUE_EXPRESSION_MACRO (SgEnumVal,isSgEnumVal,enumVal,newEnumVal)
               break;
             }
          case INT_VAL:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "INT_VAL " << implemented << endl;
#if 1
               VALUE_EXPRESSION_MACRO (SgIntVal,isSgIntVal,intVal,newIntVal)
#else
               SgIntVal* intVal = isSgIntVal(expr);
	       ROSE_ASSERT (intVal != NULL);
               Sg_File_Info* fileInfo = buildSageFileInfo();
               const int value = intVal->get_value();
               returnExpression = new SgIntVal (fileInfo,value);

	       ROSE_ASSERT (returnExpression!= NULL);
            // ROSE_ASSERT (returnExpression->get_parent() != NULL);
#endif
               break;
             }
          case UNSIGNED_INT_VAL:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "UNSIGNED_INT_VAL " << notImplemented << endl;
               VALUE_EXPRESSION_MACRO (SgUnsignedIntVal,isSgUnsignedIntVal,unsignedIntVal,newUnsignedIntVal)
               break;
             }
          case LONG_INT_VAL:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "LONG_INT_VAL " << notImplemented << endl;
               VALUE_EXPRESSION_MACRO (SgLongIntVal,isSgLongIntVal,longIntVal,newLongIntVal)
               break;
             }
          case LONG_LONG_INT_VAL:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "LONG_LONG_INT_VAL " << notImplemented << endl;
               VALUE_EXPRESSION_MACRO (SgLongLongIntVal,isSgLongLongIntVal,longLongIntVal,newLongLongIntVal)
               break;
             }
          case UNSIGNED_LONG_LONG_INT_VAL:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "UNSIGNED_LONG_LONG_INT_VAL " << notImplemented << endl;
               VALUE_EXPRESSION_MACRO (SgUnsignedLongLongIntVal,isSgUnsignedLongLongIntVal,unsignedLongLongIntVal,newUnsignedLongLongIntVal)
               break;
             }
          case UNSIGNED_LONG_INT_VAL:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "UNSIGNED_LONG_INT_VAL " << notImplemented << endl;
               VALUE_EXPRESSION_MACRO (SgUnsignedLongVal,isSgUnsignedLongVal,unsignedLongVal,newUnsignedLongVal)
               break;
             }
          case FLOAT_VAL:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "FLOAT_VAL " << notImplemented << endl;
               VALUE_EXPRESSION_MACRO (SgFloatVal,isSgFloatVal,floatVal,newFloatVal)
               break;
             }
          case DOUBLE_VAL:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "DOUBLE_VAL " << notImplemented << endl;
               VALUE_EXPRESSION_MACRO (SgDoubleVal,isSgDoubleVal,doubleVal,newDoubleVal)
               break;
             }
          case LONG_DOUBLE_VAL:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "LONG_DOUBLE_VAL " << notImplemented << endl;
               VALUE_EXPRESSION_MACRO (SgLongDoubleVal,isSgLongDoubleVal,longDoubleVal,newLongDoubleVal)
               break;
             }
          case FUNC_CALL:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "FUNC_CALL " << implemented << endl;
               SgFunctionCallExp* functionCallExp = isSgFunctionCallExp( expr );
               ROSE_ASSERT (functionCallExp != NULL);

            // modifyFunction (functionCallExp);
            // copy (functionCallExp);

               SgExpression* functionExpression = functionCallExp->get_function();
               ROSE_ASSERT (functionExpression != NULL);

               Sg_File_Info* fileInfo = buildSageFileInfo();

               SgExpression* newFunctionExpression = copy(functionExpression,oldName,newName);
               ROSE_ASSERT (newFunctionExpression != NULL);

               SgExprListExp* argList = functionCallExp->get_args();
               ROSE_ASSERT (argList != NULL);

               SgExprListExp* newArgs = (SgExprListExp*) copy (argList,oldName,newName);
               ROSE_ASSERT (newArgs != NULL);

               SgType* functionType = functionCallExp->get_type();
	       ROSE_ASSERT (functionType != NULL);

               SgType* newType = copy(functionType,oldName,newName);
               ROSE_ASSERT (newType != NULL);

               SgFunctionCallExp* newFunctionCallExp = new SgFunctionCallExp (fileInfo,newFunctionExpression,newArgs,newType);
               ROSE_ASSERT (newFunctionCallExp != NULL);
               ROSE_ASSERT (newFunctionExpression->get_parent() != NULL);

            // Why is the scope of the arguments not defined???
               ROSE_ASSERT (newArgs->get_parent() == NULL);

#if 0
               printf ("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! \n");
               printf ("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! \n");
               printf ("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! \n");
               printf ("Now unparse the old copy of the SgExpression (functionExpression) \n");
               functionExpression->logical_unparse(NO_UNPARSE_INFO,cout);
               cout << endl << endl;
	       showSgExpression ( cout, functionExpression, "Called from copy(SgFunctionCallExp*) (functionExpression)" );

               printf ("Now unparse the new copy of the SgExpression (newFunctionExpression) \n");
               newFunctionExpression->logical_unparse(NO_UNPARSE_INFO,cout);
               cout << endl << endl;
	       showSgExpression ( cout, newFunctionExpression, "Called from copy(SgFunctionExpression*) (newFunctionExpression)" );
               printf ("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! \n");
               printf ("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! \n");
               printf ("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! \n");
#endif

               returnExpression = newFunctionCallExp;
               ROSE_ASSERT (returnExpression != NULL);
               ROSE_ASSERT (returnExpression->get_parent() == NULL);
            // ROSE_ASSERT (returnExpression->get_parent() != NULL);

            // We might want to iterate through the arguments (the correct technique is to restart from the parent)???
            // SgExpression *parentExpression = functionCallExp->get_parent();
            // ROSE_ASSERT (parentExpression != NULL);
            // copy(parentExpression);
               break;
             }
          case POINTST_OP:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "POINTST_OP " << notImplemented << endl;
               BINARY_OPERATOR_MACRO (SgArrowExp,isSgArrowExp,arrowExp,newArrowExp)
               break;
             }
          case RECORD_REF:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "RECORD_REF " << implemented << endl;
#if 1
               BINARY_OPERATOR_MACRO (SgDotExp,isSgDotExp,dotExp,newDotExp)
#else
               SgDotExp* dotExp = isSgDotExp( expr );
               ROSE_ASSERT (dotExp != NULL);

               Sg_File_Info* fileInfo = buildSageFileInfo();

               SgExpression* lhsOperand = dotExp->get_lhs_operand();
               ROSE_ASSERT (lhsOperand != NULL);

               SgExpression* rhsOperand = dotExp->get_rhs_operand();
               ROSE_ASSERT (rhsOperand != NULL);

               SgExpression* lhsExpression = copy (lhsOperand,oldName,newName);
               ROSE_ASSERT (lhsExpression != NULL);

               SgExpression* rhsExpression = copy (rhsOperand,oldName,newName);
               ROSE_ASSERT (rhsExpression != NULL);

               SgType* expressionType = dotExp->get_type();
	       ROSE_ASSERT (expressionType != NULL);

               SgType* newExpressionType = copy (expressionType,oldName,newName);
               ROSE_ASSERT (newExpressionType != NULL);

               SgDotExp* newDotExp = new SgDotExp (fileInfo,lhsExpression,rhsExpression,newExpressionType);
               ROSE_ASSERT (lhsExpression->get_parent() != NULL);
               ROSE_ASSERT (rhsExpression->get_parent() != NULL);
               ROSE_ASSERT (lhsExpression->get_parent()->variant() == lhsOperand->get_parent()->variant());
               ROSE_ASSERT (rhsExpression->get_parent()->variant() == rhsOperand->get_parent()->variant());
               ROSE_ASSERT (newDotExp != NULL);
            // ROSE_ASSERT (newExpressionType->get_parent() != NULL);

            // Set the parent to be the same parent as the what we copied
               newDotExp->set_parent(dotExp->get_parent());
               ROSE_ASSERT (dotExp->get_parent()->variant() == newDotExp->get_parent()->variant());

               returnExpression = newDotExp;
               ROSE_ASSERT (returnExpression != NULL);
#endif
            // ROSE_ASSERT (returnExpression->get_parent() != NULL);
               break;
             }
          case DOTSTAR_OP:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "DOTSTAR_OP " << notImplemented << endl;
               BINARY_OPERATOR_MACRO (SgDotStarOp,isSgDotStarOp,dotStarOp,newDotStarOp)
               break;
             }
          case ARROWSTAR_OP:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "ARROWSTAR_OP " << notImplemented << endl;
               BINARY_OPERATOR_MACRO (SgArrowExp,isSgArrowExp,arrowExp,newArrowExp)
               break;
             }
          case EQ_OP:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "EQ_OP " << notImplemented << endl;
               BINARY_OPERATOR_MACRO (SgEqualityOp,isSgEqualityOp,equalityOp,newEqualityOp)
               break;
             }
          case LT_OP:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "LT_OP " << notImplemented << endl;
               BINARY_OPERATOR_MACRO (SgLessThanOp,isSgLessThanOp,lessThanOp,newLessThanOp)
               break;
             }
          case GT_OP:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "GT_OP " << notImplemented << endl;
               BINARY_OPERATOR_MACRO (SgGreaterThanOp,isSgGreaterThanOp,greaterThanOp,newGreaterThanOp)
               break;
             }
          case NE_OP:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "NE_OP " << notImplemented << endl;
               BINARY_OPERATOR_MACRO (SgNotEqualOp,isSgNotEqualOp,notEqualOp,newNotEqualOp)
               break;
             }
          case LE_OP:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "LE_OP " << implemented << endl;
#if 1
               BINARY_OPERATOR_MACRO (SgLessOrEqualOp,isSgLessOrEqualOp,lessOrEqualOp,newLessOrEqualOp)
#else
               SgLessOrEqualOp* lessOrEqualOp = isSgLessOrEqualOp (expr);
               ROSE_ASSERT (lessOrEqualOp != NULL);

               Sg_File_Info* fileInfo = buildSageFileInfo();
               SgExpression* lhsExpr = copy(lessOrEqualOp->get_lhs_operand(),oldName,newName);
               SgExpression* rhsExpr = copy(lessOrEqualOp->get_rhs_operand(),oldName,newName);
               SgType* exprType      = copy(lessOrEqualOp->get_type(),oldName,newName);
               ROSE_ASSERT (lhsExpr != NULL);
               ROSE_ASSERT (rhsExpr != NULL);
               ROSE_ASSERT (exprType != NULL);

               SgLessOrEqualOp* newLessOrEqualOp = new SgLessOrEqualOp (fileInfo,lhsExpr,rhsExpr,exprType);
               ROSE_ASSERT (newLessOrEqualOp != NULL);
	       returnExpression = newLessOrEqualOp;

               ROSE_ASSERT (returnExpression != NULL);
            // ROSE_ASSERT (returnExpression->get_parent() != NULL);
#endif
               break;
             }
          case GE_OP:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "GE_OP " << notImplemented << endl;
               BINARY_OPERATOR_MACRO (SgGreaterOrEqualOp,isSgGreaterOrEqualOp,greaterOrEqualOp,newGreaterOrEqualOp)
               break;
             }
          case ADD_OP:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "ADD_OP " << notImplemented << endl;
#if 1
               BINARY_OPERATOR_MACRO (SgAddOp,isSgAddOp,addOp,newAddOp)
#else      
               SgAddOp* addOp = isSgAddOp (expr);
               ROSE_ASSERT (addOp != NULL);

               Sg_File_Info* fileInfo = buildSageFileInfo();
               SgExpression* lhsExpr = copy(addOp->get_lhs_operand(),oldName,newName);
               SgExpression* rhsExpr = copy(addOp->get_rhs_operand(),oldName,newName);
               SgType* exprType      = copy(addOp->get_type(),oldName,newName);
               ROSE_ASSERT (lhsExpr != NULL);
               ROSE_ASSERT (rhsExpr != NULL);
               ROSE_ASSERT (exprType != NULL);

               SgAddOp* newAddOp = new SgAddOp (fileInfo,lhsExpr,rhsExpr,exprType);
               ROSE_ASSERT (newAddOp != NULL);
	       returnExpression = newAddOp;
#endif
               ROSE_ASSERT (returnExpression != NULL);
               break;
             }
          case SUBT_OP:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "SUBT_OP " << notImplemented << endl;
               BINARY_OPERATOR_MACRO (SgSubtractOp,isSgSubtractOp,subtractOp,newSubtractOp)
               break;
             }
          case MULT_OP:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "MULT_OP " << notImplemented << endl;
               BINARY_OPERATOR_MACRO (SgMultiplyOp,isSgMultiplyOp,multiplyOp,newMultiplyOp)
               break;
             }
          case DIV_OP:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "DIV_OP " << notImplemented << endl;
               BINARY_OPERATOR_MACRO (SgDivideOp,isSgDivideOp,divideOp,newDivideOp)
               break;
             }
          case INTEGER_DIV_OP:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "INTEGER_DIV_OP " << notImplemented << endl;
               BINARY_OPERATOR_MACRO (SgIntegerDivideOp,isSgIntegerDivideOp,integerDivideOp,newIntegerDivideOp)
               break;
             }
          case MOD_OP:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "MOD_OP " << notImplemented << endl;
               BINARY_OPERATOR_MACRO (SgModOp,isSgModOp,modOp,newModOp)
               break;
             }
          case AND_OP:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "AND_OP " << notImplemented << endl;
               BINARY_OPERATOR_MACRO (SgAndOp,isSgAndOp,andOp,newAndOp)
               break;
             }
          case OR_OP:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "OR_OP " << notImplemented << endl;
               BINARY_OPERATOR_MACRO (SgOrOp,isSgOrOp,orOp,newOrOp)
               break;
             }
          case BITXOR_OP:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "BITXOR_OP " << notImplemented << endl;
               BINARY_OPERATOR_MACRO (SgBitXorOp,isSgBitXorOp,bitXorOp,newBitXorOp)
               break;
             }
          case BITAND_OP:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "BITAND_OP " << notImplemented << endl;
               BINARY_OPERATOR_MACRO (SgBitAndOp,isSgBitAndOp,bitAndOp,newBitAndOp)
               break;
             }
          case BITOR_OP:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "BITOR_OP " << notImplemented << endl;
               BINARY_OPERATOR_MACRO (SgBitOrOp,isSgBitOrOp,bitOrOp,newBitOrOp)
               break;
             }
          case COMMA_OP:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "COMMA_OP " << notImplemented << endl;
               BINARY_OPERATOR_MACRO (SgCommaOpExp,isSgCommaOpExp,commaOpExp,newCommaOpExp)
               break;
             }
          case LSHIFT_OP:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "LSHIFT_OP " << notImplemented << endl;
               BINARY_OPERATOR_MACRO (SgLshiftOp,isSgLshiftOp,lshiftOp,newLshiftOp)
               break;
             }
          case RSHIFT_OP:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "RSHIFT_OP " << notImplemented << endl;
               BINARY_OPERATOR_MACRO (SgRshiftOp,isSgRshiftOp,rshiftOp,newRshiftOp)
               break;
             }
          case UNARY_MINUS_OP:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "UNARY_MINUS_OP " << notImplemented << endl;
               UNARY_OPERATOR_MACRO (SgMinusOp,isSgMinusOp,minusOp,newMinusOp)
               break;
             }
          case UNARY_ADD_OP:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "UNARY_ADD_OP " << notImplemented << endl;
               UNARY_OPERATOR_MACRO (SgUnaryAddOp,isSgUnaryAddOp,unaryAddOp,newUnaryAddOp)
               break;
             }
          case SIZEOF_OP:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "SIZEOF_OP " << notImplemented << endl;
               ROSE_ASSERT (returnExpression != NULL);
               break;
             }
          case TYPEID_OP:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "TYPEID_OP " << notImplemented << endl;
               ROSE_ASSERT (returnExpression != NULL);
               break;
             }
          case NOT_OP:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "NOT_OP " << notImplemented << endl;
               UNARY_OPERATOR_MACRO (SgNotOp,isSgNotOp,notOp,newNotOp)
               break;
             }
          case DEREF_OP:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "DEREF_OP " << notImplemented << endl;
               UNARY_OPERATOR_MACRO (SgPointerDerefExp,isSgPointerDerefExp,pointerDerefExp,newPointerDerefExp)
               break;
             }
          case ADDRESS_OP:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "ADDRESS_OP " << notImplemented << endl;
               UNARY_OPERATOR_MACRO (SgAddressOfOp,isSgAddressOfOp,addressOfOp,newAddressOfOp)
               break;
             }
          case MINUSMINUS_OP:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "MINUSMINUS_OP " << notImplemented << endl;
               UNARY_OPERATOR_MACRO (SgMinusMinusOp,isSgMinusMinusOp,minusMinusOp,newMinusMinusOp)
               break;
             }
          case PLUSPLUS_OP:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "PLUSPLUS_OP " << implemented << endl;
#if 1
               UNARY_OPERATOR_MACRO (SgPlusPlusOp,isSgPlusPlusOp,plusPlusOp,newPlusPlusOp)

            // This controls if we get a "++var" or a "var++" (it must be set after construction)
#ifdef USE_SAGE3
               SgPlusPlusOp::Sgop_mode mode = (SgPlusPlusOp::Sgop_mode) plusPlusOp->get_mode();
#else
               SgPlusPlusOp::op_mode mode = (SgPlusPlusOp::op_mode) plusPlusOp->get_mode();
#endif
               newPlusPlusOp->set_mode(mode);
#else
               SgPlusPlusOp* plusPlusOp = isSgPlusPlusOp (expr);
               ROSE_ASSERT (plusPlusOp != NULL);
               Sg_File_Info* fileInfo = buildSageFileInfo();
               ROSE_ASSERT (plusPlusOp->get_operand() != NULL);
               SgExpression* newExpr = copy (plusPlusOp->get_operand(),oldName,newName);
               ROSE_ASSERT (plusPlusOp->get_type() != NULL);
               SgType* exprType = copy (plusPlusOp->get_type(),oldName,newName);
	       ROSE_ASSERT (exprType != NULL);

               SgPlusPlusOp* newPlusPlusOp = new SgPlusPlusOp ( fileInfo, newExpr, exprType);
	       ROSE_ASSERT (newPlusPlusOp != NULL);

            // This controls if we get a "++var" or a "var++" (it must be set after construction)
               SgPlusPlusOp::op_mode mode = (SgPlusPlusOp::op_mode) plusPlusOp->get_mode();
               newPlusPlusOp->set_mode(mode);

#if 0
               printf ("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! \n");
               printf ("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! \n");
               printf ("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! \n");
               printf ("Now unparse the old version of the SgPlusPlusOP Expression (plusPlusOP) \n");
               showSgExpression ( cout, plusPlusOp , "Called from copy function" );
               plusPlusOp->logical_unparse(NO_UNPARSE_INFO,cout);
               cout << endl << endl;
               printf ("Now unparse the new copy of the SgPlusPlusOP Expression (newPlusPlusOP) \n");
               showSgExpression ( cout, newPlusPlusOp , "Called from copy function" );
               newPlusPlusOp->logical_unparse(NO_UNPARSE_INFO,cout);
               cout << endl << endl;
               printf ("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! \n");
               printf ("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! \n");
               printf ("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! \n");
               printf ("Exiting until we get the ++i vs. i++ bug fixed! \n");
               ROSE_ABORT();
#endif

               returnExpression = newPlusPlusOp;
               ROSE_ASSERT (returnExpression != NULL);
#endif
               break;
             }
#ifndef USE_SAGE3
          case ABSTRACT:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "ABSTRACT " << notImplemented << endl;
               ROSE_ASSERT (returnExpression != NULL);
               break;
             }
#endif
          case BIT_COMPLEMENT_OP:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "BIT_COMLEMENT_OP " << notImplemented << endl;
               UNARY_OPERATOR_MACRO (SgBitComplementOp,isSgBitComplementOp,bitComplementOp,newBitComplementOp)
               break;
             }
          case EXPR_CONDITIONAL:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "EXPR_CONDITIONAL " << notImplemented << endl;
               ROSE_ASSERT (returnExpression != NULL);
               break;
             }
#ifndef USE_SAGE3
          case CLASSINIT_OP:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "CLASSINIT_OP " << notImplemented << endl;
               ROSE_ASSERT (returnExpression != NULL);
               break;
             }
          case DYNAMICCAST_OP:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "DYNAMICCAST_OP " << notImplemented << endl;
               ROSE_ASSERT (returnExpression != NULL);
               break;
             }
#endif
          case CAST_OP:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "CAST_OP " << notImplemented << endl;
               UNARY_OPERATOR_MACRO (SgCastExp,isSgCastExp,castExp,newCastExp)
               break;
             }
          case ARRAY_OP:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "ARRAY_OP " << implemented << endl;
#if 1
               BINARY_OPERATOR_MACRO (SgPntrArrRefExp,isSgPntrArrRefExp,pntrArrRefExp,newPntrArrRefExp)
#else
               SgPntrArrRefExp* pntrArrRefExp = isSgPntrArrRefExp(expr);
               ROSE_ASSERT (pntrArrRefExp != NULL);

            // showSgExpression ( cout, expr, "Called from copy(expr)" );

            // Standard way of handling a binary operator
               Sg_File_Info* fileInfo = buildSageFileInfo();
               SgExpression* lhsExpr = copy(pntrArrRefExp->get_lhs_operand(),oldName,newName);
               SgExpression* rhsExpr = copy(pntrArrRefExp->get_rhs_operand(),oldName,newName);
               SgType* exprType      = copy(pntrArrRefExp->get_type(),oldName,newName);
               ROSE_ASSERT (lhsExpr != NULL);
               ROSE_ASSERT (rhsExpr != NULL);
               ROSE_ASSERT (exprType != NULL);

               SgPntrArrRefExp* newPntrArrRefExp = new SgPntrArrRefExp(fileInfo,lhsExpr,rhsExpr,exprType);
               ROSE_ASSERT (newPntrArrRefExp != NULL);

	       returnExpression = newPntrArrRefExp;

               ROSE_ASSERT (returnExpression != NULL);
               ROSE_ASSERT (returnExpression->get_parent() != NULL);
#endif
               break;
             }
          case NEW_OP:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "NEW_OP " << notImplemented << endl;
               ROSE_ASSERT (returnExpression != NULL);
               break;
             }
          case DELETE_OP:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "DELETE_OP " << notImplemented << endl;
               ROSE_ASSERT (returnExpression != NULL);
               break;
             }
          case THIS_NODE:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "THIS_NODE " << notImplemented << endl;
               ROSE_ASSERT (returnExpression != NULL);
               break;
             }
          case SCOPE_OP:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "SCOPE_OP " << notImplemented << endl;
               BINARY_OPERATOR_MACRO (SgScopeOp,isSgScopeOp,scopeOp,newScopeOp)
               break;
             }
          case ASSIGN_OP:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "ASSIGN_OP " << implemented << endl;
#if 1
               BINARY_OPERATOR_MACRO (SgAssignOp,isSgAssignOp,assignOp,newAssignOp)
#else
               SgAssignOp *assignOp = isSgAssignOp(expr);
               ROSE_ASSERT (assignOp != NULL);

            // Standard way of handling a binary operator
               Sg_File_Info* fileInfo = buildSageFileInfo();

               ROSE_ASSERT (assignOp->get_lhs_operand() != NULL);
               ROSE_ASSERT (assignOp->get_rhs_operand() != NULL);

               SgExpression* lhsExpr  = copy(assignOp->get_lhs_operand(),oldName,newName);
               SgExpression* rhsExpr  = copy(assignOp->get_rhs_operand(),oldName,newName);
               SgType*       exprType = copy(assignOp->get_type()       ,oldName,newName);

               ROSE_ASSERT (lhsExpr != NULL);
               ROSE_ASSERT (rhsExpr != NULL);
               ROSE_ASSERT (exprType != NULL);

               SgAssignOp* newAssignOp = new SgAssignOp (fileInfo,lhsExpr,rhsExpr,exprType);
               ROSE_ASSERT (newAssignOp != NULL);

               ROSE_ASSERT (lhsExpr->get_parent()  != NULL);
               ROSE_ASSERT (rhsExpr->get_parent()  != NULL);
            // ROSE_ASSERT (exprType->get_parent() != NULL);

	       returnExpression = newAssignOp;
               ROSE_ASSERT (returnExpression != NULL);
            // ROSE_ASSERT (returnExpression->get_parent() != NULL);
#endif
               break;
             }
          case PLUS_ASSIGN_OP:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "PLUS_ASSIGN_OP " << notImplemented << endl;
               BINARY_OPERATOR_MACRO (SgPlusAssignOp,isSgPlusAssignOp,plusAssignOp,newPlusAssignOp)
               break;
             }
          case MINUS_ASSIGN_OP:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "MINUS_ASSIGN_OP " << notImplemented << endl;
               BINARY_OPERATOR_MACRO (SgMinusAssignOp,isSgMinusAssignOp,minusAssignOp,newMinusAssignOp)
               break;
             }
          case AND_ASSIGN_OP:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "AND_ASSIGN_OP " << notImplemented << endl;
               BINARY_OPERATOR_MACRO (SgAndAssignOp,isSgAndAssignOp,andAssignOp,newAndAssignOp)
               break;
             }
          case IOR_ASSIGN_OP:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "IOR_ASSIGN_OP " << notImplemented << endl;
               BINARY_OPERATOR_MACRO (SgIorAssignOp,isSgIorAssignOp,iorAssignOp,newIorAssignOp)
               break;
             }
          case MULT_ASSIGN_OP:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "MULT_ASSIGN_OP " << notImplemented << endl;
               BINARY_OPERATOR_MACRO (SgMultAssignOp,isSgMultAssignOp,multAssignOp,newMultAssignOp)
               break;
             }
          case DIV_ASSIGN_OP:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "DIV_ASSIGN_OP " << notImplemented << endl;
               BINARY_OPERATOR_MACRO (SgDivAssignOp,isSgDivAssignOp,divAssignOp,newDivAssignOp)
               break;
             }
          case MOD_ASSIGN_OP:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "MOD_ASSIGN_OP " << notImplemented << endl;
               BINARY_OPERATOR_MACRO (SgModAssignOp,isSgModAssignOp,modAssignOp,newModAssignOp)
               break;
             }
          case XOR_ASSIGN_OP:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "XOR_ASSIGN_OP " << notImplemented << endl;
               BINARY_OPERATOR_MACRO (SgXorAssignOp,isSgXorAssignOp,xorAssignOp,newXorAssignOp)
               break;
             }
          case LSHIFT_ASSIGN_OP:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "LSHIFT_ASSIGN_OP " << notImplemented << endl;
               BINARY_OPERATOR_MACRO (SgLshiftAssignOp,isSgLshiftAssignOp,lshiftAssignOp,newLshiftAssignOp)
               break;
             }
          case RSHIFT_ASSIGN_OP:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "RSHIFT_ASSIGN_OP " << notImplemented << endl;
               BINARY_OPERATOR_MACRO (SgRshiftAssignOp,isSgRshiftAssignOp,rshiftAssignOp,newrshiftAssignOp)
               break;
             }
#ifndef USE_SAGE3
          case FORDECL_OP:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "FORDECL_OP " << notImplemented << endl;
               ROSE_ASSERT (returnExpression != NULL);
               break;
             }
#endif
          case TYPE_REF:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "TYPE_REF " << notImplemented << endl;
               ROSE_ASSERT (returnExpression != NULL);
               break;
             }
#ifndef USE_SAGE3
          case VECTOR_CONST:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "VECTOR_CONST " << notImplemented << endl;
               ROSE_ASSERT (returnExpression != NULL);
               break;
             }
#endif
          case EXPR_INIT:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "EXPR_INIT " << notImplemented << endl;
               ROSE_ASSERT (returnExpression != NULL);
               break;
             }
          case AGGREGATE_INIT:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "AGGREGATE_INIT " << notImplemented << endl;
               ROSE_ASSERT (returnExpression != NULL);
               break;
             }
          case CONSTRUCTOR_INIT:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "CONSTRUCTOR_INIT " << implemented << endl;
               SgConstructorInitializer* constructorInitializer = isSgConstructorInitializer( expr );
               ROSE_ASSERT (constructorInitializer != NULL);
               returnExpression = copy(constructorInitializer,oldName,newName);
               ROSE_ASSERT (returnExpression != NULL);
               break;
             }
          case ASSIGN_INIT:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "ASSIGN_INIT " << notImplemented << endl;
               ROSE_ASSERT (returnExpression != NULL);
               break;
             }
          case THROW_OP:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "THROW_OP " << notImplemented << endl;
               UNARY_OPERATOR_MACRO (SgThrowOp,isSgThrowOp,throwOp,newThrowOp)
               break;
             }
          default:
               cerr << "copy.C, copy(SgExpression) -- default reached" << endl;
        }

     ROSE_ASSERT (returnExpression != NULL);
#if USE_SAGE3
     ROSE_ASSERT (returnExpression->get_parent() == NULL);
#endif

     return returnExpression;
   }


SgType*
ROSE_TransformationBase::copy ( SgType* type, SgName oldName, SgName newName )
   {
  // Note that new types are built with shallow references to the existing internal declarations

     SgType* returnType = NULL;
     ROSE_ASSERT (type != NULL);

  // error debugging variable
     int ROSE_COPY_DEBUG = FILE_LEVEL_COPY_DEBUG_VALUE;

     char* notImplemented = "in COPY(SgType) NOT implemented";
     char* implemented    = "in COPY(SgType) IS implemented";

  // cout << "Sage Class Name: " << type->sage_class_name() << endl;

     switch (type->variant()) 
        {
          case T_UNKNOWN:	
             {
               if (ROSE_COPY_DEBUG > 0) cout << "T_UNKNOWN " << notImplemented << endl;
               ROSE_ASSERT (returnType != NULL);
               break;
             }
          case T_CHAR:	
             {
               if (ROSE_COPY_DEBUG > 0) cout << "T_CHAR " << implemented << endl;
               SgTypeChar *typeChar = isSgTypeChar (type);
               ROSE_ASSERT (typeChar != NULL);
               returnType = typeChar;
               ROSE_ASSERT (returnType != NULL);
               break;
             }
          case T_SIGNED_CHAR:	
             {
               if (ROSE_COPY_DEBUG > 0) cout << "T_SIGNED_CHAR " << notImplemented << endl;
               ROSE_ASSERT (returnType != NULL);
               break;
             }
          case T_UNSIGNED_CHAR:	
             {
               if (ROSE_COPY_DEBUG > 0) cout << "T_UNSIGNED_CHAR " << notImplemented << endl;
               ROSE_ASSERT (returnType != NULL);
               break;
             }
          case T_SHORT:	
             {
               if (ROSE_COPY_DEBUG > 0) cout << "T_SHORT " << notImplemented << endl;
               ROSE_ASSERT (returnType != NULL);
               break;
             }
          case T_SIGNED_SHORT:	
             {
               if (ROSE_COPY_DEBUG > 0) cout << "T_SIGNED_SHORT " << notImplemented << endl;
               ROSE_ASSERT (returnType != NULL);
               break;
             }
          case T_UNSIGNED_SHORT:	
             {
               if (ROSE_COPY_DEBUG > 0) cout << "T_UNSIGNED_SHORT " << notImplemented << endl;
               ROSE_ASSERT (returnType != NULL);
               break;
             }
          case T_INT:	
             {
               if (ROSE_COPY_DEBUG > 0) cout << "T_INT " << notImplemented << endl;
               SgTypeInt *typeInt = isSgTypeInt(type);
               ROSE_ASSERT (typeInt != NULL);
               returnType = typeInt;
               ROSE_ASSERT (returnType != NULL);
               break;
             }
          case T_SIGNED_INT:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "T_SIGNED_INT " << notImplemented << endl;
               SgTypeSignedInt *typeSignedInt = isSgTypeSignedInt (type);
               ROSE_ASSERT (typeSignedInt != NULL);

               returnType = new SgTypeSignedInt;
               ROSE_ASSERT (returnType != NULL);
               break;
             }
          case T_UNSIGNED_INT:	
             {
               if (ROSE_COPY_DEBUG > 0) cout << "T_UNSIGNED_INT " << notImplemented << endl;
               ROSE_ASSERT (returnType != NULL);
               break;
             }
          case T_LONG:	
             {
               if (ROSE_COPY_DEBUG > 0) cout << "T_LONG " << notImplemented << endl;
               ROSE_ASSERT (returnType != NULL);
               break;
             }
          case T_SIGNED_LONG:	
             {
               if (ROSE_COPY_DEBUG > 0) cout << "T_SIGNED_LONG " << notImplemented << endl;
               ROSE_ASSERT (returnType != NULL);
               break;
             }
          case T_UNSIGNED_LONG:	
             {
               if (ROSE_COPY_DEBUG > 0) cout << "T_UNSIGNED_LONG " << notImplemented << endl;
               ROSE_ASSERT (returnType != NULL);
               break;
             }
          case T_VOID:	
             {
               if (ROSE_COPY_DEBUG > 0) cout << "T_VOID " << notImplemented << endl;
               SgTypeVoid *typeVoid = isSgTypeVoid (type);
               ROSE_ASSERT (typeVoid != NULL);

               returnType = new SgTypeVoid;
               ROSE_ASSERT (returnType != NULL);
               break;
             }
          case T_GLOBAL_VOID:	
             {
               if (ROSE_COPY_DEBUG > 0) cout << "T_GLOBAL_VOID " << notImplemented << endl;
               SgTypeGlobalVoid *typeGlobalVoid = isSgTypeGlobalVoid (type);
               ROSE_ASSERT (typeGlobalVoid != NULL);

               returnType = new SgTypeGlobalVoid;
               ROSE_ASSERT (returnType != NULL);
               break;
             }
          case T_WCHAR:	
             {
               if (ROSE_COPY_DEBUG > 0) cout << "T_WCHAR " << notImplemented << endl;
               SgTypeWchar *typeWchar = isSgTypeWchar (type);
               ROSE_ASSERT (typeWchar != NULL);

               returnType = new SgTypeWchar;
               ROSE_ASSERT (returnType != NULL);
               break;
             }
          case T_FLOAT:	
             {
               if (ROSE_COPY_DEBUG > 0) cout << "T_FLOAT " << notImplemented << endl;
               SgTypeFloat *typeFloat = isSgTypeFloat (type);
               ROSE_ASSERT (typeFloat != NULL);

               returnType = new SgTypeFloat;
               ROSE_ASSERT (returnType != NULL);
               break;
             }
          case T_DOUBLE:	
             {
               if (ROSE_COPY_DEBUG > 0) cout << "T_DOUBLE " << notImplemented << endl;
               SgTypeDouble *typeDouble = isSgTypeDouble (type);
               ROSE_ASSERT (typeDouble != NULL);

               returnType = new SgTypeDouble;
               ROSE_ASSERT (returnType != NULL);
               break;
             }
          case T_LONG_LONG:	
             {
               if (ROSE_COPY_DEBUG > 0) cout << "T_LONG_LONG " << notImplemented << endl;
               ROSE_ASSERT (returnType != NULL);
               break;
             }
          case T_UNSIGNED_LONG_LONG:	
             {
               if (ROSE_COPY_DEBUG > 0) cout << "T_UNSIGNED_LONG_LONG " << notImplemented << endl;
               ROSE_ASSERT (returnType != NULL);
               break;
             }
          case T_LONG_DOUBLE:	
             {
               if (ROSE_COPY_DEBUG > 0) cout << "T_LONG_DOUBLE " << notImplemented << endl;
               ROSE_ASSERT (returnType != NULL);
               break;
             }
          case T_STRING:	
             {
               if (ROSE_COPY_DEBUG > 0) cout << "T_STRING " << notImplemented << endl;
               ROSE_ASSERT (returnType != NULL);
               break;
             }
          case T_BOOL:	
             {
               if (ROSE_COPY_DEBUG > 0) cout << "T_BOOL " << notImplemented << endl;
               SgTypeBool *typeBool = isSgTypeBool (type);
               ROSE_ASSERT (typeBool != NULL);

               returnType = new SgTypeBool;
               ROSE_ASSERT (returnType != NULL);
               break;
             }
          case T_COMPLEX:	
             {
               if (ROSE_COPY_DEBUG > 0) cout << "T_COMPLEX " << notImplemented << endl;
               ROSE_ASSERT (returnType != NULL);
               break;
             }
          case T_DEFAULT:	
             {
               if (ROSE_COPY_DEBUG > 0) cout << "T_DEFAULT " << notImplemented << endl;
               ROSE_ASSERT (returnType != NULL);
               break;
             }
          case T_POINTER:	
             {
               if (ROSE_COPY_DEBUG > 0) cout << "T_POINTER " << notImplemented << endl;
               SgPointerType *typePointer = isSgPointerType (type);
               ROSE_ASSERT (typePointer != NULL);

               SgType* oldBaseType = typePointer->get_base_type();
	       ROSE_ASSERT (oldBaseType != NULL);

               SgType* newBaseType = copy (oldBaseType,oldName,newName);
	       ROSE_ASSERT (newBaseType != NULL);

               SgPointerType* newPointerType = new SgPointerType (newBaseType);
               ROSE_ASSERT (newPointerType != NULL);

               returnType = newPointerType;
               ROSE_ASSERT (returnType != NULL);
               break;
             }
          case T_MEMBER_POINTER:	
             {
               if (ROSE_COPY_DEBUG > 0) cout << "T_MEMBER_POINTER " << notImplemented << endl;
               SgPointerMemberType *pointerMemberType = isSgPointerMemberType (type);
               ROSE_ASSERT (pointerMemberType != NULL);
               ROSE_ASSERT (returnType != NULL);
               break;
             }
          case T_REFERENCE:	
             {
               if (ROSE_COPY_DEBUG > 0) cout << "T_REFERENCE " << notImplemented << endl;
               SgReferenceType *referenceType = isSgReferenceType(type);
               ROSE_ASSERT (referenceType != NULL);

               SgType *oldBaseType = referenceType->get_base_type();
               ROSE_ASSERT (oldBaseType != NULL);

               SgType* newBaseType = copy (oldBaseType,oldName,newName);
               ROSE_ASSERT (newBaseType != NULL);

               SgReferenceType* newReferenceType = new SgReferenceType (newBaseType);
               ROSE_ASSERT (newReferenceType != NULL);

               returnType = newReferenceType;
               ROSE_ASSERT (returnType != NULL);
               break;
             }
          case T_NAME:	
             {
               if (ROSE_COPY_DEBUG > 0) cout << "T_NAME " << notImplemented << endl;
               ROSE_ASSERT (returnType != NULL);
               break;
             }
          case T_CLASS:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "T_CLASS " << notImplemented << endl;
               SgClassType* classType = isSgClassType( type );
               ROSE_ASSERT (classType != NULL);

               SgDeclarationStatement* oldDeclaration = classType->get_declaration();
               ROSE_ASSERT (oldDeclaration != NULL);

            // Build new type with shallow reference to oldDeclaration
               SgClassType* newType = new SgClassType (oldDeclaration);
               ROSE_ASSERT (newType != NULL);

               returnType = newType;
               ROSE_ASSERT (returnType != NULL);
               break;
             }
#ifndef USE_SAGE3
          case T_STRUCT:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "T_STRUCT " << notImplemented << endl;
               ROSE_ASSERT (returnType != NULL);
               break;
             }
#endif
          case T_ENUM:	
             {
               if (ROSE_COPY_DEBUG > 0) cout << "T_ENUM " << notImplemented << endl;
               ROSE_ASSERT (returnType != NULL);
               break;
             }
#ifndef USE_SAGE3
          case T_UNION:	
             {
               if (ROSE_COPY_DEBUG > 0) cout << "T_UNION " << notImplemented << endl;
               ROSE_ASSERT (returnType != NULL);
               break;
             }
#endif
          case T_TYPEDEF:	
             {
               if (ROSE_COPY_DEBUG > 0) cout << "T_TYPEDEF " << notImplemented << endl;
               ROSE_ASSERT (returnType != NULL);
               break;
             }
          case T_MODIFIER:	
             {
               if (ROSE_COPY_DEBUG > 0) cout << "T_MODIFIER " << notImplemented << endl;
               SgModifierType *typeModifier = isSgModifierType (type);
               ROSE_ASSERT (typeModifier != NULL);

               SgType* oldBaseType = typeModifier->get_base_type();
	       ROSE_ASSERT (oldBaseType != NULL);

            // copy the bitField
               int bitField = typeModifier->get_bitfield();

               SgType* newBaseType = copy (oldBaseType,oldName,newName);
	       ROSE_ASSERT (newBaseType != NULL);

               SgModifierType* newTypeModifier = new SgModifierType (newBaseType,bitField);
               ROSE_ASSERT (newTypeModifier != NULL);

#if (ROSE_USE_RESTRICT_KEYWORD == FALSE)
            // Code specific to introduction of "restrict" keyword (buried in the type information)
            // We want to turn off the restrict keyword if it is not supported
               if ( newTypeModifier->isRestrict() )
                    newTypeModifier->unsetRestrict();
#endif
               returnType = newTypeModifier;
               ROSE_ASSERT (returnType != NULL);
               break;
             }
          case T_FUNCTION:	
             {
               if (ROSE_COPY_DEBUG > 0) cout << "T_FUNCTION " << notImplemented << endl;
               SgFunctionType* functionType = isSgFunctionType( type );
               ROSE_ASSERT (functionType != NULL);

               SgType* oldReturnType =  functionType->get_return_type();
	       ROSE_ASSERT (oldReturnType != NULL);
	       SgType* newReturnType = copy (oldReturnType,oldName,newName);

            // These don't have to be copied!
               sage_bool hasEllipses                = functionType->get_has_ellipses();
               SgFunctionType* newFunctionType = new SgFunctionType(newReturnType,hasEllipses);
	       ROSE_ASSERT (newFunctionType != NULL);

            // Set a few more details the same as what we are trying to copy
               newFunctionType->set_orig_return_type(functionType->get_orig_return_type());
	       returnType = newFunctionType;
               ROSE_ASSERT (returnType != NULL);
               break;
             }
          case T_MEMBERFUNCTION:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "T_MEMBERFUNCTION " << notImplemented << endl;
               SgMemberFunctionType* memberFunctionType = isSgMemberFunctionType( type );
               ROSE_ASSERT (memberFunctionType != NULL);

               SgType* oldReturnType =  memberFunctionType->get_return_type();
	       ROSE_ASSERT (oldReturnType != NULL);
	       SgType* newReturnType = copy (oldReturnType,oldName,newName);

            // These don't have to be copied!
               sage_bool hasEllipses                = memberFunctionType->get_has_ellipses();
	       SgClassDefinition* structName        = memberFunctionType->get_struct_name();
	       unsigned int memberFunctionSpecifier = memberFunctionType->get_mfunc_specifier();

            // showSgStatement (cout, structName, "Called from COPY(SgType)" );

               ROSE_ASSERT (structName != NULL);

               SgMemberFunctionType* newMemberFunctionType = new SgMemberFunctionType(newReturnType,hasEllipses,structName, memberFunctionSpecifier);
	       ROSE_ASSERT (newMemberFunctionType != NULL);

            // SAGE BUG: There is some sort of bug in Sage such that the functionDefinition pointer is overwritten
            // in this case we can just reset it directly
               ROSE_ASSERT (newMemberFunctionType->get_struct_name() == NULL);
	       newMemberFunctionType->set_struct_name(structName);
               ROSE_ASSERT (newMemberFunctionType->get_struct_name() != NULL);
               ROSE_ASSERT (newMemberFunctionType->get_mfunc_specifier() == 0);
               ROSE_ASSERT (newMemberFunctionType->get_mfunc_specifier() != memberFunctionSpecifier);
               newMemberFunctionType->set_mfunc_specifier(memberFunctionSpecifier);
               ROSE_ASSERT (newMemberFunctionType->get_mfunc_specifier() == memberFunctionSpecifier);	       

            // Set a few more details the same as what we are trying to copy
               newMemberFunctionType->set_orig_return_type(memberFunctionType->get_orig_return_type());
#if 0
               showSgType (cout, memberFunctionType, "Called from COPY(memberFunctionType)" );
               showSgType (cout, newMemberFunctionType, "Called from COPY(newMemberFunctionType)" );
#endif
	       returnType = newMemberFunctionType;
               ROSE_ASSERT (returnType != NULL);
               break;
             }
          case T_PARTIAL_FUNCTION:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "T_PARTIAL_FUNCTION " << notImplemented << endl;
               ROSE_ASSERT (returnType != NULL);
               break;
             }
          case T_ARRAY:	
             {
			   // CW: make a type cast...
               SgArrayType *arrayType = isSgArrayType( type );
               ROSE_ASSERT (arrayType != NULL);
               
               // CW: to create a new SgArrayType I need the basic type of
               // the array and an expression which describes the index expression
               SgType *baseType=arrayType->get_base_type();
			   SgExpression *indexExpr=arrayType->get_index();
			   ROSE_ASSERT(baseType!=NULL);
			   ROSE_ASSERT(indexExpr!=NULL); // might be to strict !?!

			   // CW: I have to copy both recursively		   
			   SgType *newBaseType=copy(baseType,oldName,newName);
			   SgExpression *newIndexExpr=copy(indexExpr,oldName,newName);
			   ROSE_ASSERT(newBaseType);
			   ROSE_ASSERT(newIndexExpr);
			   
			   // CW: now I can build the new SgArrayType
			   SgArrayType *newArrayType = new SgArrayType(newBaseType,newIndexExpr);
               ROSE_ASSERT (newArrayType != NULL);

               returnType = newArrayType;
               break;
             }
          case T_ELLIPSE:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "T_ELLIPSE " << notImplemented << endl;
               ROSE_ASSERT (returnType != NULL);
               break;
             }
          default:
             cout << "copy.C, copy(SgType)--default reached" << endl;
        } // switch

     ROSE_ASSERT (returnType != NULL);
     return returnType;
   }


SgInitializedName*
ROSE_TransformationBase::copy ( SgInitializedName *initializedName, SgName oldName, SgName newName )
   {
     SgInitializedName* returnInitializedName = NULL;
     ROSE_ASSERT (initializedName != NULL);

  // Make a copy of the string to prevent side-effects when the name is changed (edited)
  // const char* originalName = initializedName->get_name().str();
     const char* originalName = strdup(initializedName->get_name().str());

     char* copyOfOriginalName = new char [128];
     ROSE_ASSERT (copyOfOriginalName != NULL);

     char* result             = NULL;

     ROSE_ASSERT (originalName  != NULL);
     ROSE_ASSERT (oldName.str() != NULL);

  // printf ("In ROSE/ExamplePreProcessors/A++Preprocessor/TransformBaseClass: In ROSE_TransformationBase::copy(SgInitializedName*,SgName*,SgName) \n");

  // Look for a name we can replace with a new name!
     if ( (!oldName.is_null()) && (ROSE::isSameName(originalName,oldName.str()) == TRUE) )
        {
          result = strcpy(copyOfOriginalName,newName.str());
#if 0
	  printf ("##################################################################################################################################### \n");
	  printf ("##################################################################################################################################### \n");
	  printf ("##################################################################################################################################### \n");
	  printf ("##################################################################################################################################### \n");
          printf ("In ROSE_TransformationBase::copy ( SgInitializedName *initializedName,oldName = %s,newName = %s ) Found a name to copy! %s  --> %s \n",
               oldName,newName,newName,oldName);
	  printf ("##################################################################################################################################### \n");
	  printf ("##################################################################################################################################### \n");
	  printf ("##################################################################################################################################### \n");
	  printf ("##################################################################################################################################### \n");
       // ROSE_ABORT();
#endif
        }
       else
        {
          result = strcpy(copyOfOriginalName,originalName);
        }
#if 0
     printf ("In ROSE_TransformationBase::copy ( SgInitializedName *initializedName,oldName = %s,newName = %s ) Found a name to copy! (%s) --> (%s) \n",
          oldName,newName,originalName,copyOfOriginalName);

     if (ROSE::isSameName(originalName,"LHS_ARRAY") == TRUE)
        {
          printf ("Exiting from copy(SgInitializedName)! ... \n");
          ROSE_ABORT();
        }
#endif

     ROSE_ASSERT (result != NULL);

     SgName newNameObject (copyOfOriginalName);

     ROSE_ASSERT (initializedName->get_type() != NULL);
     SgType* oldType                                 = initializedName->get_type();
     ROSE_ASSERT (oldType != NULL);
     SgInitializer* oldInitializer                   = initializedName->get_initializer();
     ROSE_ASSERT (oldInitializer == NULL);
  // ROSE_ASSERT (oldInitializer != NULL);
     SgDeclarationStatement* oldDeclarationStatement = initializedName->get_declaration();
     ROSE_ASSERT (oldDeclarationStatement != NULL);
     SgInitializedName* oldInitializedName           = initializedName->get_named_item();
     ROSE_ASSERT (oldInitializedName != NULL);
     SgInitializedName* oldPreviousInitializedName   = initializedName->get_prev_decl_item();
     ROSE_ASSERT (oldPreviousInitializedName == NULL);
  // ROSE_ASSERT (oldPreviousInitializedName != NULL);
     
     SgType* newType                                 = copy(oldType,oldName,newName);
     ROSE_ASSERT (newType != NULL);

     ROSE_ASSERT (oldInitializer == NULL);
     SgInitializer* newInitializer = NULL;
     if (oldInitializer != NULL)
        {
          newInitializer = copy(oldInitializer,oldName,newName);
          ROSE_ASSERT (newInitializer != NULL);
        }

  // SgDeclarationStatement* newDeclarationStatement = (SgDeclarationStatement*) copy(oldDeclarationStatement,oldName,newName);
     SgDeclarationStatement* newDeclarationStatement = oldDeclarationStatement;
     ROSE_ASSERT (newDeclarationStatement != NULL);
  // SgInitializedName* newInitializedName           = copy(oldInitializedName,oldName,newName);
     SgInitializedName* newInitializedName           = oldInitializedName;
     ROSE_ASSERT (newInitializedName != NULL);

     ROSE_ASSERT (oldPreviousInitializedName == NULL);
     SgInitializedName* newPreviousInitializedName   = NULL;
     if (oldPreviousInitializedName != NULL)
        {
          newPreviousInitializedName   = copy(oldPreviousInitializedName,oldName,newName);
          ROSE_ASSERT (newPreviousInitializedName != NULL);
        }
     
  // I'm not sure if we should reuse the existing "get_initializer()"
  // Also, not sure if we should use the existing "get_declaration()", 
  // "get_named_item()" and "get_prev_decl_item()"
     returnInitializedName = new SgInitializedName
                                ( newNameObject,
                                  newType,
                                  newInitializer,
                                  newDeclarationStatement,
                                  newInitializedName,
                                  newPreviousInitializedName);

// initializedName->get_initializer(),
// initializedName->get_declaration(),
// initializedName->get_named_item(),
// initializedName->get_prev_decl_item());

     ROSE_ASSERT (returnInitializedName != NULL);

  // showSgInitializedName ( cout, returnInitializedName , "Called from copy function" );

#if 0
     printf ("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! \n");
     printf ("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! \n");
     printf ("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! \n");
     printf ("Now unparse the old copy of the SgInitializer Statement (initializedName) \n");
     initializedName.logical_unparse(NO_UNPARSE_INFO,cout);
     cout << endl << endl;
     printf ("Now unparse the new copy of the SgInitializer Statement (returnInitializedName) \n");
     returnInitializedName->logical_unparse(NO_UNPARSE_INFO,cout);
     cout << endl << endl;
     printf ("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! \n");
     printf ("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! \n");
     printf ("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! \n");
#endif

     return returnInitializedName;
   }

SgSymbol*
ROSE_TransformationBase::copy ( SgSymbol* sym, SgName oldName, SgName newName )
   {
     SgSymbol* returnSymbol = NULL;
     ROSE_ASSERT (sym != NULL);

  // error debugging variable
     int ROSE_COPY_DEBUG = FILE_LEVEL_COPY_DEBUG_VALUE;

     char* notImplemented = "in COPY(SgSymbol) NOT implemented";
     char* implemented    = "in COPY(SgSymbol) IS implemented";

  // cout << "Sage Class Name: " << sym->sage_class_name() << endl;

     switch (sym->variant()) 
        {
          case VARIABLE_NAME:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "VARIABLE_NAME " << notImplemented << endl;
               SgVariableSymbol* variableSymbol = isSgVariableSymbol( sym );
               ROSE_ASSERT (variableSymbol != NULL);
               SgInitializedName* initializedName = variableSymbol->get_declaration();
               ROSE_ASSERT (initializedName != NULL);

            // showSgInitializedName(cout, initializedName);

               SgInitializedName* newInitializedName = copy(initializedName,oldName,newName);
               ROSE_ASSERT (newInitializedName != NULL);

               SgVariableSymbol* newVariableSymbol = new SgVariableSymbol (newInitializedName);
               ROSE_ASSERT (newVariableSymbol != NULL);

               returnSymbol = newVariableSymbol;
	       ROSE_ASSERT (returnSymbol != NULL);
               break;
             }
          case FUNCTION_NAME:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "FUNCTION_NAME " << notImplemented << endl;
               SgFunctionSymbol* functionSymbol = isSgFunctionSymbol( sym );
               SgFunctionDeclaration* functionDeclaration = functionSymbol->get_declaration();

            // showSgStatement(cout,  functionDeclaration, "Inside of copy(sym)");

               SgFunctionDeclaration* newFunctionDeclaration = functionDeclaration;

               SgFunctionSymbol* newFunctionSymbol = new SgFunctionSymbol(newFunctionDeclaration);
               ROSE_ASSERT (newFunctionSymbol != NULL);

               returnSymbol = newFunctionSymbol;
               ROSE_ASSERT (returnSymbol != NULL);
               break;
             }
          case FUNCTYPE_NAME:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "FUNCTYPE_NAME " << notImplemented << endl;
               ROSE_ASSERT (returnSymbol != NULL);
               break;
             }
          case CLASS_NAME:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "CLASS_NAME " << notImplemented << endl;
               ROSE_ASSERT (returnSymbol != NULL);
               break;
             }
#ifndef USE_SAGE3
          case UNION_NAME:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "UNION_NAME " << notImplemented << endl;
               ROSE_ASSERT (returnSymbol != NULL);
               break;
             }
          case STRUCT_NAME:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "STRUCT_NAME " << notImplemented << endl;
               ROSE_ASSERT (returnSymbol != NULL);
               break;
             }
#endif
          case ENUM_NAME:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "ENUM_NAME " << notImplemented << endl;
               ROSE_ASSERT (returnSymbol != NULL);
               break;
             }
          case FIELD_NAME:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "FIELD_NAME " << notImplemented << endl;
               ROSE_ASSERT (returnSymbol != NULL);
               break;
             }
          case TYPEDEF_NAME:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "TYPEDEF_NAME " << notImplemented << endl;
               ROSE_ASSERT (returnSymbol != NULL);
               break;
             }
          case MEMBER_FUNC_NAME:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "MEMBER_FUNC_NAME " << implemented << endl;
               SgMemberFunctionSymbol* memberFunctionSymbol = isSgMemberFunctionSymbol( sym );
#if 0
            // from SgFunctionSymbol
               SgFunctionDeclaration* functionDeclaration = memberFunctionSymbol->get_declaration();
               ROSE_ASSERT (functionDeclaration != NULL);
#endif
            // from SgMemberFunctionSymbol
               SgMemberFunctionDeclaration* memberFunctionDeclaration = memberFunctionSymbol->get_declaration();
               ROSE_ASSERT (memberFunctionDeclaration != NULL);

#if 0
               SgMemberFunctionDeclaration* newMemberFunctionDeclaration = 
                    (SgMemberFunctionDeclaration*) copy(memberFunctionDeclaration,oldName,newName);
#else
            // Don't copy the declaration
               SgMemberFunctionDeclaration* newMemberFunctionDeclaration = memberFunctionDeclaration;
#endif
            // SgClassDefinition* classDefinition = memberFunctionSymbol->get_scope();

               SgMemberFunctionSymbol* newMemberFunctionSymbol = new SgMemberFunctionSymbol (newMemberFunctionDeclaration);
	       ROSE_ASSERT (newMemberFunctionSymbol != NULL);

               returnSymbol = newMemberFunctionSymbol;
               ROSE_ASSERT (returnSymbol != NULL);
               break;
             }
          case LABEL_NAME:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "LABEL_NAME " << notImplemented << endl;
               ROSE_ASSERT (returnSymbol != NULL);
               break;
             }
#ifndef USE_SAGE3
          case CONSTRUCT_NAME:
             {
               if (ROSE_COPY_DEBUG > 0) cout << "CONSTRUCT_NAME " << notImplemented << endl;
               ROSE_ASSERT (returnSymbol != NULL);
               break;
             }
#endif
          default:
               cout << "copy.C, copy(SgSymbol) -- default reached" << endl;
        } // switch

     ROSE_ASSERT (returnSymbol != NULL);
     return returnSymbol;
   }




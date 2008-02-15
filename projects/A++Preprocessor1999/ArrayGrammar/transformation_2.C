#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "rose.h"

#if 0
#include "array_class_interface.h"
#include "../TransformBaseClass/TransformationBase.h"
#include "ROSE_Transformations.h"
#endif

#include "ROSE_Transformations.h"

// include "transformation_2.h"

// This file represents the parser for the following grammar:
//      transformableExpression :=
//           ( transformableExpression operator transformableExpression )
//           | ( operand "operator=" transformableExpression ) .

// The implementation is as a top-down predictive parser (the look ahead is fixed because of the
// simplicity of the grammar we are using).

// Their are well defined steps:
//  1) verify that we have an expression statement
//     (this is done in parsing the SgStatement (the root of the Sage Tree that we deal with here).
//  2) look ahead to see if the first ROSE_Node will be an "operator=" of an array class.
//  3) Build the object representing the ROSE_ArrayOperatorEquals
//  4) look ahead to see if the lhs is an operand (as defined by our grammar (the A++/P++ grammar))
//  5) parse the lhs and build a ROSE_ArrayOperand object
//  6) parse the rhs and build the ROSE_Expression associated with the rhs
//  7) recursively parse the rhs

// In the event of a parse error (i.e. we discoved that we can't parse the statement after all)
// we must recover and cleanup the ROSE_Node objects that we have built.

// The recovery proceedure: ??? (See the Dragon book for alternatives)
// The simple method is to use "Panic-mode recovery", we would implement this
// by returning a NULL pointer (of type ROSE_Node*).  This would trigger
// the deletion of all objects constructed at any node in the ROSE parse tree
// and the final return of a NULL pointer to the orginal call to parse
// ("ROSE_Node* parse ( SgStatement* stmt )").
// The problem is made a bit more complex since an error during the constructor call for
// any of the ROSE non-terminal objects (in our grammar) must be caught after the construction.
// To handle this we provide an error state within the ROSE_Node base class and this state is
// checked after construction of the non-terminal objects in our grammar.


ArrayAssignmentUsingGrammar::~ArrayAssignmentUsingGrammar ()
   {
   // No variables to delete
   }

ArrayAssignmentUsingGrammar::ArrayAssignmentUsingGrammar ( SgFile *file )
   : ROSE_TransformationBase(file)
   {
  // Nothing for this constructor to do except to call the base class constructor
   }

// This function is a Virtual function in the base class (ROSE_TransformationBase)
ROSE_StatementBlock*
ArrayAssignmentUsingGrammar::pass( ROSE_StatementBlock* inputRoseProgramTree )
   {
  // A Pass is what calls all the functions to parse and transform a given file

  // There is nothing that is expected to be input to this function (we were forced to 
  // include an input parameter because of a virtual function "pass" in the base class)
     ROSE_ASSERT (inputRoseProgramTree == NULL);

     ROSE_StatementBlock* roseProgramTree = traverseProgramTree();
     ROSE_ASSERT (roseProgramTree != NULL);

  // Now perform the transformations
  // transform functions will be called after the next pass using the transformation grammar
  // transform (roseProgramTree);

  // Return the program tree
     ROSE_ASSERT (roseProgramTree != NULL);
     return roseProgramTree;
   }

// It is not clear yet how much of this function can be placed into the base class
// we will pursue that later when we have one transformation working and we want
// to develop more transformations.
ROSE_StatementBlock*
ArrayAssignmentUsingGrammar::traverseProgramTree ()
   {
  // This function traverses the statements within the program tree,
  // identifies those statements that are a part of the ROSE input file,
  // evaluates them for the transformation represented by this 
  // class (SimpleArrayAssignment), and transforms the statements.

     SgFile *file          = getProgramFile();
     char   *inputFileName = ROSE::getFileName(file);

#ifdef ROSE_INTERNAL_DEBUG
     if ( (ROSE_DEBUG > 0) || (ROSE::verbose == TRUE) )
          printf ("Inside of ArrayAssignmentUsingGrammar::traverseProgramTree() for file = %s \n",inputFileName);
#endif

     char outputFilename[256];
     sprintf(outputFilename,"%s.roseShow_t2",inputFileName);
     fstream ROSE_ShowFile(outputFilename,ios::out);

     if ( (ROSE_DEBUG > 0) || (ROSE::verbose == TRUE) )
          printf ("Program tree output Filename (for C++ (Sage II) grammar) = %s \n",outputFilename);

  // Select an output stream for the program tree display (cout or <filename>.C.roseShow)
  // Macro OUTPUT_SHOWFILE_TO_FILE is defined in the transformation_1.h header file
     ostream & outputStream = (OUTPUT_SHOWFILE_TO_FILE ? ((ostream&) ROSE_ShowFile) : ((ostream&) cout));

  // We start by getting the SgScopeStatement and the using its iterator
  // to go through the statements in that scope.
     SgScopeStatement *globalScope = (SgScopeStatement *)(&(file->root()));
     ROSE_ASSERT (globalScope != NULL);
     ROSE_ASSERT (isSgScopeStatement(globalScope) != NULL);

  // SgBasicBlock* basicBlock = NULL;
  // ROSE_StatementBlock* roseGlobalStatementBlock = new ROSE_StatementBlock (basicBlock);
     ROSE_StatementBlock* roseGlobalStatementBlock = new ROSE_StatementBlock (globalScope);
     ROSE_ASSERT (roseGlobalStatementBlock != NULL);

  // Loop though the statements in the global scope
     for (SgStatement::iterator s_iter = globalScope->begin();
          s_iter != globalScope->end(); s_iter++)
        {
       // Check if the statement from the original file (not the include files)
       // We are only interested in transforming statements in the input file, later
       // we can broaden our reach to functions included from other header files etc.
       // if (isSameName(fname, fileName) == TRUE)
          if (ROSE::isSameName(inputFileName, ROSE::getFileName(*s_iter)) == TRUE)
             {
#if 0
            // For this transformation we don't care if *s_iter is an array class declaration!
               if ( ArrayClassSageInterface::IsArrayClassDeclaration ( *s_iter ) )
                  {
                    if (ROSE_DEBUG > -1)
                         printf ("File Name %s -- Line Number %d -- FOUND AN A++ DECLARATION! \n",
                              ROSE::getFileName(*s_iter), ROSE::getLineNumber(*s_iter) );
                  }
#endif
#if 0
            // print out the showfile associated with the program tree generated for the input statement
               if (ROSE::outputGrammarTreeFiles == TRUE)
                    showSgStatement ( outputStream, *s_iter, "SimpleArrayAssignment::traverseProgramTree()");
            // showSgStatement ( cout, *s_iter, "SimpleArrayAssignment::traverseProgramTree()");
#endif

            // check if an assignment expression statement
            // Turn on and off internal ROSE debugging!
               ROSE_DEBUG = (ROSE::getLineNumber(*s_iter) == -1) ? ROSE_DEBUG : ROSE_DEBUG;

            // We have not yet delt with what we will do with this arrayStatement
            // This is too high a level to do the transformation on the array statement 
            // since at this level it could represent a function with a statement block.
            // We need to address the transformation phase at a lower level where we 
            // don't see the statement block.
               ROSE_Statement* roseStatement = parse (*s_iter);

#if ROSE_INTERNAL_DEBUG
               if ( (ROSE_DEBUG > 0) || (ROSE::verbose == TRUE) )
                  {
                    if (roseStatement == NULL)
                       {
                         printf ("\n\n");
                         printf ("################################################################################ \n");
                         printf ("################################################################################ \n");
                         printf ("Inside of ArrayAssignmentUsingGrammar::traverseProgramTree()"
                                 " File: %s  Line: %d is NOT an Array Statement that we recognize (do nothing) \n",
                              ROSE::getFileName(*s_iter),ROSE::getLineNumber(*s_iter));
                         printf ("################################################################################ \n");
                         printf ("################################################################################ \n");
                         printf ("\n\n");

                      // printf ("arrayStatement == NULL, we have some sot of error using our grammar (or something not implemented)! \n");
                      // ROSE_ABORT();
                       }
                  }
#endif

            // Now put the new statement into the list of statements stored in the statement block
               roseGlobalStatementBlock->addArrayStatementToBlock(roseStatement);

             } // end of if ROSE::isSameName(,)
        } // end of "for" loop

  // printf("Done with ArrayAssignmentUsingGrammar::traverseProgramTree () \n");
     ROSE_ASSERT (roseGlobalStatementBlock != NULL);
     return roseGlobalStatementBlock;
   }


ROSE_Statement*
ArrayAssignmentUsingGrammar::parse ( SgFunctionDeclaration *functionDeclaration )
   {
  // This function could be located in the BASECLASS
     ROSE_Statement* returnNode = NULL;
   
     ROSE_ASSERT (functionDeclaration != NULL);
     SgFunctionDefinition *functionDefinition = functionDeclaration->get_definition();
     ROSE_ASSERT (functionDefinition != NULL);
     SgBasicBlock *basicBlock = functionDefinition->get_body();

     if (basicBlock != NULL)
          returnNode = parse(basicBlock);
       else
        {
          cout << "(basicBlock == NULL) (function body definition not available): ";
          ROSE_ABORT();
        }
     
     return returnNode;
   }

ROSE_Statement*
ArrayAssignmentUsingGrammar::parse ( SgMemberFunctionDeclaration *memberFunctionDeclaration )
   {
  // This function could be located in the BASECLASS (???)
     ROSE_Statement* returnNode = NULL;

     ROSE_ASSERT (memberFunctionDeclaration != NULL);
     SgFunctionDefinition *functionDefinition = memberFunctionDeclaration->get_definition();
     ROSE_ASSERT (functionDefinition != NULL);
     SgFunctionDeclaration* functionDeclarationStatement = functionDefinition->get_declaration();
     ROSE_ASSERT (functionDeclarationStatement != NULL);

  // Call parse for the function declaration
     returnNode = parse (functionDeclarationStatement);

     return returnNode;
   }

ROSE_Statement*
ArrayAssignmentUsingGrammar::parse ( SgForStatement *forStatement )
   {
     ROSE_ASSERT (forStatement != NULL);

     ROSE_Statement* returnNode = NULL;

  // get the variant so we can distinguish between where statements, elsewhere statements, and regular for loops
  // ArrayGrammarTerminalVariants forVariant = ArrayClassSageInterface::getArrayLibGeneralFunctionVariant(forStatement);
     ROSE_GrammarVariants forVariant = ROSE_ForStatementTag;
     
  // Print out the body (list of statements) for the "for" loop
     ROSE_ASSERT (forStatement->get_loop_body() != NULL);
     SgBasicBlock *basicBlock = forStatement->get_loop_body();
     ROSE_ASSERT (basicBlock != NULL);

     ROSE_StatementBlock* roseStatementBlock = parse (basicBlock);
     ROSE_ASSERT (roseStatementBlock != NULL);

  // We need to reconginze that an elsewhere statement must 
  // follow either a where statement or another elsewhere statement     
     switch (forVariant)
        {
          case ROSE_WhereStatementTag:
             {
               ROSE_WhereStatement* roseWhereStatement = new ROSE_WhereStatement(forStatement);
	       ROSE_ASSERT (roseWhereStatement != NULL);

            // error recover mechanism
               if (roseWhereStatement->error())
                  {
                 // We have detected an error found in parsing the Sage tree to build the ROSE_WhereStatement
                    returnNode = NULL;
                  }
                 else
                  {
                    roseWhereStatement->setRoseWhereStatementBlock (roseStatementBlock);
                    returnNode = roseWhereStatement;
                  }

               break;
             }
          case ROSE_ElseWhereStatementTag:
             {
               ROSE_ElseWhereStatement* roseElseWhereStatement = new ROSE_ElseWhereStatement(forStatement);
	       ROSE_ASSERT (roseElseWhereStatement != NULL);

            // error recover mechanism
               if (roseElseWhereStatement->error())
                  {
                 // We have detected an error found in parsing the Sage tree to build the ROSE_ElseWhereStatement
                    returnNode = NULL;
                  }
                 else
                  {
                    roseElseWhereStatement->setRoseElseWhereStatementBlock (roseStatementBlock);
                    returnNode = roseElseWhereStatement;
                  }

               break;
             }
          case ROSE_ForStatementTag:
             {
               ROSE_ForStatement* roseForStatement = new ROSE_ForStatement(forStatement);
	       ROSE_ASSERT (roseForStatement != NULL);

            // error recover mechanism
               if (roseForStatement->error())
                  {
                 // We have detected an error found in parsing the Sage tree to build the ROSE_ForStatement
                    returnNode = NULL;
                  }
                 else
                  {
                    roseForStatement->setRoseForStatementBlock (roseStatementBlock);
                    returnNode = roseForStatement;
                  }

               break;
             }
          default:
               printf ("ERROR: default reached in ArrayAssignmentUsingGrammar::parse ( SgForStatement *forStatement ) \n");
               ROSE_ABORT();
        }

     ROSE_ASSERT (returnNode != NULL);
     return returnNode;
   }

ROSE_StatementBlock*
ArrayAssignmentUsingGrammar::parse ( SgBasicBlock *basicBlock )
   {
     ROSE_StatementBlock* returnNode = NULL;

     ROSE_ASSERT (basicBlock != NULL);
     SgStatementPtrList::iterator statementIterator;

  // cout << "Number of statements in block = " << basicBlock->get_statements().size() << endl;

  // Save the current block so that a statement to be transformated can see its context in that block
  // a SgBasicBlock is derived from a SgScopeStatement
     pushCurrentScope (basicBlock);

     ROSE_StatementBlock* roseStatementBlock = new ROSE_StatementBlock (basicBlock);
     ROSE_ASSERT (roseStatementBlock != NULL);

     for (statementIterator = basicBlock->get_statements().begin();
          statementIterator != basicBlock->get_statements().end();
          statementIterator++)
        {
       // Get the statement
#if USE_SAGE3
          SgStatement* sageStatement = (*statementIterator);
#else
          SgStatement* sageStatement = (*statementIterator).irep();
#endif

       // Get the SgStatement from the SgStatementPrt using the irep() member function
          ROSE_Statement* roseStatement = parse ( sageStatement );
          ROSE_ASSERT (roseStatement != NULL);

       // Now put the new statement into the list of statements stored in the statement block
          roseStatementBlock->addArrayStatementToBlock(roseStatement);
        }

  // Save the current block so that a statement to be transformated can see its context in that block
  // a SgBasicBlock is derived from a SgScopeStatement
     popCurrentScope (basicBlock);

  // returnNode = roseStatementBlock;

  // error recover mechanism
     if (roseStatementBlock->error())
        {
       // We have detected an error found in parsing the Sage tree to build the ROSE_ArrayParenthesisOperator
          returnNode = NULL;
        }
       else
        {
          returnNode = roseStatementBlock;
        }

  // This is temporary code
     ROSE_ASSERT (returnNode != NULL);
     return returnNode;
   }

ROSE_Statement*
ArrayAssignmentUsingGrammar::parse ( SgStatement* stmt )
   {
  // Used for debugging unimplemented parts of the pragma recognition mechanism
     int ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG = ((ROSE_DEBUG > -1) || (ROSE::verbose == TRUE)) ? 1 : 0;

     ROSE_Statement* returnNode = NULL;
     ROSE_ASSERT (stmt != NULL);

#if ROSE_INTERNAL_DEBUG
     if ( (ROSE_DEBUG > -1) || (ROSE::verbose == TRUE) )
        {
          printf ("In ArrayAssignmentUsingGrammar::parse ( SgStatement = %s ) Line: %d File: %s \n",
               ROSE::identifyVariant(stmt->variant()),ROSE::getLineNumber(stmt),ROSE::getFileName(stmt));

       // showSgStatement ( cout,stmt, "CALLED FROM ArrayAssignmentUsingGrammar::parse(stmt)");
        }
#endif

     char* notImplemented = "Not implemented in ArrayAssignmentUsingGrammar::parse ( SgStatement* stmt )";

  // Actually we want to switch based upon our grammar, it is just a little 
  // less important at the beginning of our work.  So we can use the Sage 
  // elements for now!
     switch (stmt->variant())
        {
          case EXPR_STMT:
             {
            // cout << "EXPR_STMT implemented" << endl;
               SgExprStatement *exprStatement = isSgExprStatement(stmt);
	       ROSE_ASSERT (exprStatement != NULL);

#if 0
            // Here is the look ahead mechanism, we turn this off for now!

            // We might not want to strip the expression from the SgStatement
            // Now that we have the statement representing an expression
            // (a SgExprStatement) hand that off to the validation mechanism
            // without stripping out the SgExpression inside!
               if (validateTarget (cout, stmt))
                  {
                // then continue to parse the statement/expression
                  }
#endif

            // Part of parsing using our restricted grammar for the assignment 
            // operation is to look ahead and see if the first ROSE_Node (the 
            // first node in our expression tree) will be an "operator=" for 
            // an array class.

            // We make this type ROSE_Statement instead of ROSE_ExpressionStatement 
            // because it might have to be converted to a ROSE_C_Statement we can't 
            // tell if this is going to be required until we parse the full expression!
               ROSE_ExpressionStatement* arrayStatement = new ROSE_ExpressionStatement (stmt);
               ROSE_ASSERT (arrayStatement != NULL);

            // error recover mechanism
               if (arrayStatement->error())
                  {
                 // We have detected an error found in parsing the Sage tree to build the ROSE_ArrayParenthesisOperator
                    printf ("ERROR detected in building ROSE_ArrayStatement (in parser) \n");
                    ROSE_ABORT();

                    returnNode = NULL;
                  }
                 else
                  {
                 // Found an expression Statement (that is the first step)
                    SgExpression *expr = exprStatement->get_the_expr();
                    ROSE_ASSERT (expr != NULL);

                 // Now parse the expression attached to this statement
                 // printf ("Calling ArrayAssignmentUsingGrammar::parse ( SgExpression* expr ) in "
                 //         "ArrayAssignmentUsingGrammar::parse ( SgStatement* stmt ) \n");

                    ROSE_Expression* roseExpression = (ROSE_Expression*) parse (expr);
                    ROSE_ASSERT (roseExpression != NULL);

                 // We can't assert that the arrayExpression is not NULL because returning NULL 
                 // is part of the triggering of our error recovery mechanism.
                 // ROSE_ASSERT (arrayExpression != NULL);

                 // Error recovery mechanism
                 // if (roseExpression == NULL)
                    if (roseExpression->error())
                       {
                      // Skip the calls to the decstructors for now and accept the memory leak
                      // delete arrayStatement;
                         arrayStatement = NULL;
                       }
                      else
                       {
                      // The only other problem we should account for is the case where 
                      // the roseExpression is a ROSE_C_Expression since then we don't 
                      // want to consider this to be a ROSE_ExpressionStatement, we would 
                      // want to build a ROSE_C_Statement.  We handle this as a special case!
                      // This handles the case of "printf();" which was previously recognized 
                      // as a ROSE_ExpressionStatement rather than a ROSE_C_Statement.
                      // ROSE_ASSERT (roseExpression->getVariant() != ROSE_C_ExpressionTag);
                         if (roseExpression->getVariant() == ROSE_C_ExpressionTag)
                            {
                           // This is a memory leak, which we accept for now!
                              arrayStatement = NULL;
                              ROSE_C_Statement* roseC_Statement = new ROSE_C_Statement (stmt);
                              ROSE_ASSERT (roseC_Statement != NULL);
			      ROSE_C_Expression* rose_C_Expression = (ROSE_C_Expression*) roseExpression;
                              roseC_Statement->setRoseExpression(rose_C_Expression);
                              returnNode = roseC_Statement;
                            }
                           else
                            {
                              arrayStatement->setRoseExpression(roseExpression);
                              returnNode = arrayStatement;
                            }
                       }
                  }

               ROSE_ASSERT (returnNode != NULL);
               break;
             }
          case FUNC_DECL_STMT:
             {
            // cout << "MFUNC_DECL_STMT implemented" << endl;
               returnNode = parse (isSgFunctionDeclaration(stmt));
               ROSE_ASSERT (returnNode != NULL);
               break;
             }
          case MFUNC_DECL_STMT:
             {
            // cout << "MFUNC_DECL_STMT implemented" << endl;
               returnNode = parse (isSgMemberFunctionDeclaration(stmt));
               ROSE_ASSERT (returnNode != NULL);
               break;
             }
          case FOR_STMT:
             {
            // cout << "FOR_STMT implemented" << endl;

               SgForStatement* forStatement = isSgForStatement (stmt);
               ROSE_ASSERT (forStatement != NULL);

            // returnNode = parse (isSgForStatement (stmt));
               returnNode = parse (forStatement);
               ROSE_ASSERT (returnNode != NULL);
               break;
             }

       // Cases not yet handled (not all need be handled)!
       // That these are not handled means that any array operation that is a part of one of these
       // statements cannot yet be recognized as an array statement that could be optimized.
       // The following would not be caught, for example:
       //   1) min(A+B);
       //   2) return A+B;
       //   3) foo(A+B);
       //   4) { A = B + C; }
       //   5) int x = min(A+B);
       //   We will flush out the implementation as we develop ROSE further

          case DECL_STMT:
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "DECL_STMT " << notImplemented << endl;
               break;
          case SCOPE_STMT:
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "SCOPE_STMT " << notImplemented << endl;
               break;
          case FUNC_TBL_STMT:
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "FUNC_TBL_STMT " << notImplemented << endl;
               break;
          case GLOBAL_STMT:
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "GLOBAL_STMT " << notImplemented << endl;
               break;
          case BASIC_BLOCK_STMT:
             {
            // if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "BASIC_BLOCK_STMT " << notImplemented << endl;
               SgBasicBlock* basicBlock = isSgBasicBlock (stmt);
               ROSE_ASSERT (basicBlock != NULL);

            // ROSE_StatementBlock* roseStatementBlock = new ROSE_StatementBlock (basicBlock);
               ROSE_StatementBlock* roseStatementBlock = parse (basicBlock);
	       ROSE_ASSERT (roseStatementBlock != NULL);

            // error recover mechanism
               if (roseStatementBlock->error())
                  {
                 // We have detected an error found in parsing the Sage tree to build the ROSE_ArrayParenthesisOperator
                    returnNode = NULL;
                  }
                 else
                  {
                    returnNode = roseStatementBlock;
                  }

               ROSE_ASSERT (returnNode != NULL);
               break;
             }
          case IF_STMT:
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout<< "IF_STMT " << notImplemented << endl;

            // showSgStatement (cout, stmt, "Called from parse(SgStatement*)");

               SgIfStmt* ifStatement = isSgIfStmt ( stmt );
               ROSE_ASSERT (ifStatement != NULL);

               SgStatement* conditionalStatement = ifStatement->get_conditional();
               ROSE_ASSERT (conditionalStatement != NULL);

               parse (conditionalStatement);

               SgBasicBlock* trueBody = ifStatement->get_true_body();
               ROSE_ASSERT (trueBody != NULL);

            // Set the returnNode here and ignore it for the falseBody until we add a ROSE_IfStatement
               returnNode = parse(trueBody);

               SgBasicBlock* falseBody = ifStatement->get_false_body();
               if (falseBody != NULL)
                  {
                    parse(falseBody);
                  }

#if 0
            // Uncomment this once we have a ROSE_IfStatement object in our grammar
               ROSE_IfStatement* roseIfStatement = new ROSE_IfStatement(ifStatement);
	       ROSE_ASSERT (roseIfStatement != NULL);

            // error recover mechanism
               if (roseIfStatementBlock->error())
                  {
                 // We have detected an error found in parsing the Sage tree to build the ROSE_ArrayParenthesisOperator
                    returnNode = NULL;
                  }
                 else
                  {
                    returnNode = roseIfStatementBlock;
                  }
#endif
               ROSE_ASSERT (returnNode != NULL);
               break;
             }

          case FUNC_DEFN_STMT:
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "FUNC_DEFN_STMT " << notImplemented << endl;
               break;

		case VAR_DECL_STMT:
		{
			ROSE_VariableDeclaration* roseVariableDeclaration = NULL;
		
			// CW: added creation of a ROSE_ArrayVariableDeclaration node
			if(ArrayClassSageInterface::isROSEArrayVariableDeclaration(stmt)){
				roseVariableDeclaration = new ROSE_ArrayVariableDeclaration(stmt);
			}
			else{
				roseVariableDeclaration = new ROSE_C_VariableDeclaration(stmt);				
			}

			ROSE_ASSERT(roseVariableDeclaration != NULL);

			// error recover mechanism
			if (roseVariableDeclaration->error())
			{
				// We have detected an error found in parsing the Sage tree to build the ROSE_ArrayParenthesisOperator
				returnNode = NULL;
			}
			else
			{
				returnNode = roseVariableDeclaration;
			}

			ROSE_ASSERT(returnNode != NULL);
			break;
		}
          case VAR_DEFN_STMT:
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "VAR_DEFN_STMT " << notImplemented << endl;
               break;
          case CLASS_DECL_STMT:
             {
            // if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "CLASS_DECL_STMT " << notImplemented << endl;

               ROSE_C_Statement* roseClassDeclarationStatement = NULL;

            // Check if this is to be interpreted as par tof the array grammar (should be fals for this case)
#if 0
               if (ArrayClassSageInterface::isROSEArrayVariableDeclaration(stmt))
                  {
                    roseClassDeclarationStatement = new ROSE_ArrayClassDeclaration(stmt);
                  }
                 else
                  {
                    roseClassDeclarationStatement = new ROSE_C_Statement(stmt);
                  }
#else
               roseClassDeclarationStatement = new ROSE_C_Statement(stmt);
#endif

	       ROSE_ASSERT (roseClassDeclarationStatement != NULL);

            // error recover mechanism
               if (roseClassDeclarationStatement->error())
                  {
                 // We have detected an error found in parsing the Sage tree to build the ROSE_ArrayParenthesisOperator
                    returnNode = NULL;
                  }
                 else
                  {
                    returnNode = roseClassDeclarationStatement;
                  }

               ROSE_ASSERT (returnNode != NULL);
               break;
             }
          case CLASS_DEFN_STMT:
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "CLASS_DEFN_STMT " << notImplemented << endl;
               break;
          case ENUM_DECL_STMT:
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ENUM_DECL_STMT " << notImplemented << endl;
               break;
          case LABEL_STMT:
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "LABEL_STMT " << notImplemented << endl;
               break;
          case WHILE_STMT:
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "WHILE_STMT " << notImplemented << endl;
               break;
          case DO_WHILE_STMT:
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "DO_WHILE_STMT " << notImplemented << endl;
               break;
          case SWITCH_STMT:
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "SWITCH_STMT " << notImplemented << endl;
               break;
          case CASE_STMT:
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "CASE_STMT " << notImplemented << endl;
               break;
          case TRY_STMT:
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "TRY_STMT " << notImplemented << endl;
               break;
          case CATCH_STMT:
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "CATCH_STMT " << notImplemented << endl;
               break;
          case DEFAULT_STMT:
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "DEFAULT_STMT " << notImplemented << endl;
               break;
          case BREAK_STMT:
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "BREAK_STMT " << notImplemented << endl;
               break;
          case CONTINUE_STMT:
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "CONTINUE_STMT " << notImplemented << endl;
               break;
          case RETURN_STMT:
             {
            // if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "RETURN_STMT " << notImplemented << endl;

            // We declare a return statement to be a C Statement for now 
            // (later we can check if it is an Array Return Statement)
               ROSE_C_Statement* roseReturnStatement = new ROSE_C_Statement(stmt);
	       ROSE_ASSERT (roseReturnStatement != NULL);

            // error recover mechanism
               if (roseReturnStatement->error())
                  {
                 // We have detected an error found in parsing the Sage tree to build the ROSE_ArrayParenthesisOperator
                    returnNode = NULL;
                  }
                 else
                  {
                    returnNode = roseReturnStatement;
                  }

               ROSE_ASSERT (returnNode != NULL);
               break;
             }
          case GOTO_STMT:
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "GOTO_STMT " << notImplemented << endl;
               break;
          case ASM_STMT:
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ASM_STMT " << notImplemented << endl;
               break;
          case SPAWN_STMT:
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "SPAWN_STMT " << notImplemented << endl;
               break;
          case PAR_STMT:
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "PAR_STMT " << notImplemented << endl;
               break;
          case PARFOR_STMT:
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "PARFOR_STMT " << notImplemented << endl;
               break;
          case TYPEDEF_STMT:
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "TYPEDEF_STMT " << notImplemented << endl;
               break;
          case TEMPLATE_DECL_STMT:
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "TEMPLATE_DECL_STMT " << notImplemented << endl;
               break;

          default:
               cout << "tranformation_2.C, parse(SgStatement*) -- default reached" << endl;
               ROSE_ABORT();
        }

     ROSE_ASSERT (returnNode != NULL);
  // ROSE_ASSERT (returnNode != NULL);  // Temp code which disables part of the error recovery mechanism
     return returnNode;
   }

ROSE_Expression*
ArrayAssignmentUsingGrammar::parse ( SgExpression* expr )
   {
  // error debugging variable
     int ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG = ( (ROSE_DEBUG > 0) || (ROSE::verbose == TRUE) ) ? 1 : 0;

     ROSE_Expression* returnNode = NULL;

     ROSE_ASSERT (expr != NULL);

#if ROSE_INTERNAL_DEBUG
     if ( (ROSE_DEBUG > 0) || (ROSE::verbose == TRUE) )
        {
       // printf ("Inside of ArrayAssignmentUsingGrammar::parse ( SgExpression* expr ) \n");
          printf ("In ArrayAssignmentUsingGrammar::parse ( SgExpression = %s ) Line: %d File: %s \n",
               ROSE::identifyVariant(expr->variant()),ROSE::getLineNumber(expr),ROSE::getFileName(expr));
        }
#endif

  // We are parsing the SAGE tree but we are classifying it first using the ArrayClassSageInterface
  // member functions.  Thus this function can organize it's switch statement over the elements of 
  // the ROSE Grammar.
  // ArrayGrammarTerminalVariants arrayClassVariant = ArrayClassSageInterface::getArrayLibGeneralFunctionVariant(expr);
     ROSE_GrammarVariants arrayClassVariant = ArrayClassSageInterface::getVariant(expr);

     char* notImplemented = "Not implemented in ArrayAssignmentUsingGrammar::parse ( SgExpression* expr )";
     char* implemented    =  "IS implemented in ArrayAssignmentUsingGrammar::parse ( SgExpression* expr )";

  // Note that while we have provided cases for every possible piece of the A++/P++ grammar, not all 
  // of them are pieces that can be returned by the getVariant(expr) function.  This is because many 
  // of these are base classes in the implementation of the grammar and so they should never be built
  // in a correctly working parser.

     switch (arrayClassVariant)
        {
          case ROSE_NodeTag:
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_NodeTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_IndexArgumentListTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_IndexArgumentListTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_IndexExpressionTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_IndexExpressionTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_IndexExpressionBinaryOperatorCExpressionTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_IndexExpressionBinaryOperatorCExpressionTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_CExpressionBinaryOperatorIndexExpressionTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_CExpressionBinaryOperatorIndexExpressionTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_UnaryOperatorIndexOperandTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_UnaryOperatorIndexOperandTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_IndexOperandTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_IndexOperandTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_IndexingVariableTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_IndexingVariableTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_IndexVariableTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_IndexVariableTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_RangeVariableTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_RangeVariableTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_IndexingVariableReferenceTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_IndexingVariableReferenceTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_IndexVariableReferenceTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_IndexVariableReferenceTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_RangeVariableReferenceTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_RangeVariableReferenceTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_IndexingVariablePointerTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_IndexingVariablePointerTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_IndexVariablePointerTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_IndexVariablePointerTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_RangeVariablePointerTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_RangeVariablePointerTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_TypeTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_TypeTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_ArrayTypeTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_ArrayTypeTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_RealArrayTypeTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_RealArrayTypeTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_doubleArrayTypeTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_doubleArrayTypeTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_floatArrayTypeTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_floatArrayTypeTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_intArrayTypeTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_intArrayTypeTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_IndexingTypeTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_IndexingTypeTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_IndexTypeTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_IndexTypeTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_RangeTypeTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_RangeTypeTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_ArgumentListTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_ArgumentListTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_StatementTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_StatementTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_VariableDeclarationTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_VariableDeclarationTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_C_VariableDeclarationTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_C_VariableDeclarationTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_ArrayVariableDeclarationTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_ArrayVariableDeclarationTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_StatementBlockTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_StatementBlockTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_C_StatementTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_C_StatementTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_ArrayStatementTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_ArrayStatementTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_ExpressionStatementTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_ExpressionStatementTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_ReturnStatementTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_ReturnStatementTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_WhereStatementTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_WhereStatementTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_ElseWhereStatementTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_ElseWhereStatementTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_DoWhileStatementTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_DoWhileStatementTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_WhileStatementTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_WhileStatementTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_ForStatementTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_ForStatementTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_IfStatementTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_IfStatementTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_ExpressionTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_ExpressionTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_UserFunctionTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_UserFunctionTag " << implemented << endl;
            // This part of the array grammar will be removed, for now treat it as a ROSE_C_Expression
             }
          case ROSE_C_ExpressionTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_C_ExpressionTag " << implemented << endl;
            // printf ("Found a C expression \n");

               ROSE_C_Expression* arrayCExpression = new ROSE_C_Expression (expr);
	       ROSE_ASSERT (arrayCExpression != NULL);

            // error recover mechanism
               if (arrayCExpression->error())
                  {
                 // We have detected an error found in parsing the Sage tree to build the ROSE_ArrayParenthesisOperator
                    returnNode = NULL;
                  }
                 else
                  {
                 // We need to find the different parts of the C_Expression and parse them too!
                    if ( (ROSE_DEBUG > 0) || (ROSE::verbose == TRUE) )
                         printf ("### WARNING: Not finished parsing the C_Expression! \n");

                    returnNode = arrayCExpression;
                  }
               break;
             }

          case ROSE_ArrayExpressionTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_ArrayExpressionTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_ArrayOperatorTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_ArrayOperatorTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_UnaryArrayOperatorTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_UnaryArrayOperatorTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_UnaryArrayOperatorMinusTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_UnaryArrayOperatorMinusTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_UnaryArrayOperatorPlusTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_UnaryArrayOperatorPlusTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_UnaryArrayOperatorPrefixPlusPlusTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_UnaryArrayOperatorPrefixPlusPlusTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_UnaryArrayOperatorPostfixPlusPlusTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_UnaryArrayOperatorPostfixPlusPlusTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_UnaryArrayOperatorPrefixMinusMinusTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_UnaryArrayOperatorPrefixMinusMinusTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_UnaryArrayOperatorPostfixMinusMinusTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_UnaryArrayOperatorPostfixMinusMinusTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_UnaryArrayLogicalOperatorTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_UnaryArrayLogicalOperatorTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_UnaryArrayOperatorNOTTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_UnaryArrayOperatorNOTTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_BinaryArrayOperatorTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_BinaryArrayOperatorTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_BinaryArrayOperatorEqualsTag: 
             {
            // if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_BinaryArrayOperatorEqualsTag " << notImplemented << endl;
               ROSE_BinaryArrayOperatorEquals* arrayOperatorEquals = new ROSE_BinaryArrayOperatorEquals (expr);
	       ROSE_ASSERT (arrayOperatorEquals != NULL);

            // error recover mechanism
               if (arrayOperatorEquals->error())
                  {
                 // We have detected an error found in parsing the Sage tree to build the ROSE_ArrayParenthesisOperator
                    returnNode = NULL;

                    printf ("Error detected in parsing the Sage tree to build the ROSE_BinaryArrayOperatorEquals(expr)! \n");
		    ROSE_ABORT();
                  }
                 else
                  {
                    SgExpression* lhsSageExpression = arrayOperatorEquals->getLhsSageExpression();
                    ROSE_ASSERT (lhsSageExpression != NULL);
#if 0
                    printf ("DOES THIS LOOK LIKE THE CORRECT LHS? \n");
                    showSgExpression ( cout,lhsSageExpression, "CALLED FROM ArrayAssignmentUsingGrammar::parse(expr)");
#endif
                    SgExpression* rhsSageExpression = arrayOperatorEquals->getRhsSageExpression();
                    ROSE_ASSERT (rhsSageExpression != NULL);
#if 0
                    printf ("DOES THIS LOOK LIKE THE CORRECT RHS? \n");
                    showSgExpression ( cout,rhsSageExpression, "CALLED FROM ArrayAssignmentUsingGrammar::parse(expr)");
#endif
                 // printf ("Follow the LHS of the expression tree first! \n");
                    ROSE_ArrayOperandExpression* lhsRoseExpression = (ROSE_ArrayOperandExpression*) parse (lhsSageExpression);
                    ROSE_ASSERT (lhsRoseExpression != NULL);

                 // printf ("Follow the RHS of the expression tree next! \n");
                    ROSE_ArrayExpression* rhsRoseExpression = (ROSE_ArrayExpression*) parse (rhsSageExpression);
                    ROSE_ASSERT (rhsRoseExpression != NULL);

#if 0
                    printf ("Exiting in ArrayAssignmentUsingGrammar::parse(expr) to verify correct LHS and RHS \n");
		    ROSE_ABORT();
#endif
                    arrayOperatorEquals->setLhsRoseExpression (lhsRoseExpression);
                    arrayOperatorEquals->setRhsRoseExpression (rhsRoseExpression);

                    returnNode = arrayOperatorEquals;
		  }
               break;
             }

          case ROSE_BinaryArrayOperatorAddTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_BinaryArrayOperatorAddTag " << notImplemented << endl;
            // printf ("Found an ARRAY_FUNC_ADD_OP \n");

               ROSE_BinaryArrayOperatorAdd* arrayAddOperator = new ROSE_BinaryArrayOperatorAdd (expr);
	       ROSE_ASSERT (arrayAddOperator != NULL);

            // error recover mechanism
               if (arrayAddOperator->error())
                  {
                 // We have detected an error found in parsing the Sage tree to build the ROSE_ArrayParenthesisOperator
                    returnNode = NULL;
                  }
                 else
                  {
                 // Build a reference to the expression list
                    SgExprListExp* sageExprListExp = arrayAddOperator->getSageExpressionListExpression();

                    SgExpressionPtrList & expressionPtrList = sageExprListExp->get_expressions();
                    SgExpressionPtrList::iterator i = expressionPtrList.begin();
                    if (i == expressionPtrList.end())
                       {
                      // cout << "<EMPTY LIST>" << endl;
                       }
                      else
                       {
                         int counter = 0;
                         while (i != expressionPtrList.end()) 
                            {
#if USE_SAGE3
                              SgExpression* expression = *i;
#else
                              SgExpressionPtr expressionPtr = *i;
                              SgExpression* expression = expressionPtr.operator->();
#endif
                           // addArgumentExpression (expression);
                              ROSE_Expression* roseExpression = parse ( expression );
                              ROSE_ASSERT (roseExpression != NULL);
                              arrayAddOperator->addArgumentExpression (roseExpression);
                              i++;
                              counter++;  // We really don't need this counter
                            }
                       }

                 // printf ("number of arguments = %d \n",arrayAddOperator->numberOfArguments());
                    returnNode = arrayAddOperator;
                  }
               break;
             }

          case ROSE_BinaryArrayOperatorAddEqualsTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_BinaryArrayOperatorAddEqualsTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_BinaryArrayOperatorMinusTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_BinaryArrayOperatorMinusTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_BinaryArrayOperatorMinusEqualsTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_BinaryArrayOperatorMinusEqualsTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_BinaryArrayOperatorMultiplyTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_BinaryArrayOperatorMultiplyTag " << notImplemented << endl;
            // printf ("Found an ARRAY_FUNC_MULTIPLY_OP \n");

               ROSE_BinaryArrayOperatorMultiply* arrayMultiplyOperator = new ROSE_BinaryArrayOperatorMultiply (expr);
	       ROSE_ASSERT (arrayMultiplyOperator != NULL);

            // error recover mechanism
               if (arrayMultiplyOperator->error())
                  {
                 // We have detected an error found in parsing the Sage tree to build the ROSE_ArrayParenthesisOperator
                    returnNode = NULL;
                  }
                 else
                  {
                 // Build a reference to the expression list
                    SgExprListExp* sageExprListExp = arrayMultiplyOperator->getSageExpressionListExpression();

                    SgExpressionPtrList & expressionPtrList = sageExprListExp->get_expressions();
                    SgExpressionPtrList::iterator i = expressionPtrList.begin();
                    if (i == expressionPtrList.end())
                       {
                      // cout << "<EMPTY LIST>" << endl;
                       }
                      else
                       {
                         int counter = 0;
                         while (i != expressionPtrList.end()) 
                            {
#if USE_SAGE3
                              SgExpression* expression = *i;
#else
                              SgExpressionPtr expressionPtr = *i;
                              SgExpression* expression = expressionPtr.operator->();
#endif
                           // addArgumentExpression (expression);
                              ROSE_Expression* roseExpression = parse ( expression );
                              ROSE_ASSERT (roseExpression != NULL);
                              arrayMultiplyOperator->addArgumentExpression (roseExpression);
                              i++;
                              counter++;  // We really don't need this counter
                            }
                       }

                 // printf ("number of arguments = %d \n",arrayMultiplyOperator->numberOfArguments());
                    returnNode = arrayMultiplyOperator;
                  }
               break;
             }
          case ROSE_BinaryArrayOperatorMultiplyEqualsTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_BinaryArrayOperatorMultiplyEqualsTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_BinaryArrayOperatorDivideTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_BinaryArrayOperatorDivideTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_BinaryArrayOperatorDivideEqualsTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_BinaryArrayOperatorDivideEqualsTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_BinaryArrayOperatorModuloTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_BinaryArrayOperatorModuloTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_BinaryArrayOperatorModuloEqualsTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_BinaryArrayOperatorModuloEqualsTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_BinaryArrayTestingOperatorTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_BinaryArrayTestingOperatorTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_BinaryArrayOperatorLTTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_BinaryArrayOperatorLTTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_BinaryArrayOperatorLTEqualsTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_BinaryArrayOperatorLTEqualsTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_BinaryArrayOperatorGTTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_BinaryArrayOperatorGTTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_BinaryArrayOperatorGTEqualsTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_BinaryArrayOperatorGTEqualsTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_BinaryArrayLogicalOperatorTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_BinaryArrayLogicalOperatorTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_BinaryArrayOperatorEquivalenceTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_BinaryArrayOperatorEquivalenceTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_BinaryArrayOperatorNOTEqualsTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_BinaryArrayOperatorNOTEqualsTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_BinaryArrayOperatorLogicalANDTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_BinaryArrayOperatorLogicalANDTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_BinaryArrayOperatorLogicalORTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_BinaryArrayOperatorLogicalORTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_NumericExpressionTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_NumericExpressionTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_ArrayOperandBinaryArrayOperatorArrayOperandTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_ArrayOperandBinaryArrayOperatorArrayOperandTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_ArrayOperandBinaryArrayOperatorArrayExpressionTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_ArrayOperandBinaryArrayOperatorArrayExpressionTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_UnaryArrayOperatorArrayOperandTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_UnaryArrayOperatorArrayOperandTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_UnaryArrayOperatorArrayExpressionTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_UnaryArrayOperatorArrayExpressionTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_TestingExpressionTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_TestingExpressionTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_ArrayOperandBinaryArrayTestingOperatorArrayOperandTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_ArrayOperandBinaryArrayTestingOperatorArrayOperandTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_ArrayOperandBinaryArrayTestingOperatorArrayExpressionTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_ArrayOperandBinaryArrayTestingOperatorArrayExpressionTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_LogicalExpressionTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_LogicalExpressionTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_ArrayOperandBinaryArrayLogicalOperatorArrayOperandTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_ArrayOperandBinaryArrayLogicalOperatorArrayOperandTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_ArrayOperandBinaryArrayLogicalOperatorArrayExpressionTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_ArrayOperandBinaryArrayLogicalOperatorArrayExpressionTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_ArrayOperandExpressionTag: 
             {
            // This is a base class and we should neveer build an object of this type!
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_ArrayOperandExpressionTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_ArrayVariableExpressionTag: 
             {
            // This is a base class and we should neveer build an object of this type!
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_ArrayVariableExpressionTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_RealArrayVariableExpressionTag: 
             {
            // This is a base class and we should neveer build an object of this type!
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_RealArrayVariableExpressionTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_doubleArrayVariableExpressionTag: 
             {
            // if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_doubleArrayVariableExpressionTag " << notImplemented << endl;
               ROSE_doubleArrayVariableExpression* doubleArrayVariableExpression = new ROSE_doubleArrayVariableExpression (expr);
               ROSE_ASSERT (doubleArrayVariableExpression != NULL);
		  
            // error recover mechanism
               if (doubleArrayVariableExpression->error())
                  {
                 // We have detected an error found in parsing the Sage tree to build the ROSE_ArrayParenthesisOperator
                    returnNode = NULL;
                  }
                 else
                  {
                 // Nothing to orgainize here that I can think of
                 // Later we will organize the aliasing information
                    returnNode = doubleArrayVariableExpression;
                  }

               ROSE_ASSERT (returnNode->getVariant() == ROSE_doubleArrayVariableExpressionTag);
               break;
             }
          case ROSE_floatArrayVariableExpressionTag: 
             {
            // if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_floatArrayVariableExpressionTag " << notImplemented << endl;
               ROSE_floatArrayVariableExpression* floatArrayVariableExpression = new ROSE_floatArrayVariableExpression (expr);
               ROSE_ASSERT (floatArrayVariableExpression != NULL);

            // error recover mechanism
               if (floatArrayVariableExpression->error())
                  {
                 // We have detected an error found in parsing the Sage tree to build the ROSE_ArrayParenthesisOperator
                    returnNode = NULL;
                  }
                 else
                  {
                 // Nothing to orgainize here that I can think of
                 // Later we will organize the aliasing information
                    returnNode = floatArrayVariableExpression;
                  }

               ROSE_ASSERT (returnNode->getVariant() == ROSE_floatArrayVariableExpressionTag);

            // printf ("Case ROSE_floatArrayType not implemented! \n");
            // ROSE_ABORT();
               break;
             }
          case ROSE_intArrayVariableExpressionTag: 
             {
            // if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_intArrayVariableExpressionTag " << notImplemented << endl;
               ROSE_intArrayVariableExpression* intArrayVariableExpression = new ROSE_intArrayVariableExpression (expr);
               ROSE_ASSERT (intArrayVariableExpression != NULL);

            // error recover mechanism
               if (intArrayVariableExpression->error())
                  {
                 // We have detected an error found in parsing the Sage tree to build the ROSE_ArrayParenthesisOperator
                    returnNode = NULL;
                  }
                 else
                  {
                 // Nothing to orgainize here that I can think of
                 // Later we will organize the aliasing information
                    returnNode = intArrayVariableExpression;
                  }

               ROSE_ASSERT (returnNode->getVariant() == ROSE_intArrayVariableExpressionTag);

            // printf ("Case ROSE_intArrayType not implemented! \n");
            // ROSE_ABORT();
               break;
             }
          case ROSE_ArrayVariableReferenceExpressionTag:
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_ArrayVariableReferenceExpressionTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_RealArrayVariableReferenceExpressionTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_RealArrayVariableReferenceExpressionTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_doubleArrayVariableReferenceExpressionTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_doubleArrayVariableReferenceExpressionTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_floatArrayVariableReferenceExpressionTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_floatArrayVariableReferenceExpressionTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_intArrayVariableReferenceExpressionTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_intArrayVariableReferenceExpressionTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_ArrayVariablePointerExpressionTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_ArrayVariablePointerExpressionTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_RealArrayVariablePointerExpressionTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_RealArrayVariablePointerExpressionTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_doubleArrayVariablePointerExpressionTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_doubleArrayVariablePointerExpressionTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_floatArrayVariablePointerExpressionTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_floatArrayVariablePointerExpressionTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_intArrayVariablePointerExpressionTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_intArrayVariablePointerExpressionTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_ArrayParenthesisOperatorTag: 
             {
            // if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_ArrayParenthesisOperatorTag " << notImplemented << endl;
            // printf ("Found an ARRAY_FUNC_PARENTHESIS_OP \n");

               ROSE_ArrayParenthesisOperator* arrayParenthesisOperator = new ROSE_ArrayParenthesisOperator (expr);
	       ROSE_ASSERT (arrayParenthesisOperator != NULL);

            // error recover mechanism
               if (arrayParenthesisOperator->error())
                  {
                 // We have detected an error found in parsing the Sage tree to build the ROSE_ArrayParenthesisOperator
                    returnNode = NULL;
                  }
                 else
                  {
#if 0
                    SgVarRefExpression* varRefExpression = arrayParenthesisOperator->operandExpression();
	            ROSE_ASSERT (operandSageExpression != NULL);
#endif
                    returnNode = arrayParenthesisOperator;
                  }
               break;
             }

          case ROSE_ArrayFunctionExpressionTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_ArrayFunctionExpressionTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_ArrayCOSTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_ArrayCOSTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_ArraySINTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_ArraySINTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_ArrayTANTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_ArrayTANTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_ArrayMINTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_ArrayMINTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_ArrayMAXTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_ArrayMAXTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_ArrayFMODTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_ArrayFMODTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_ArrayMODTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_ArrayMODTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_ArrayPOWTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_ArrayPOWTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_ArraySIGNTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_ArraySIGNTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_ArrayLOGTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_ArrayLOGTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_ArrayLOG10Tag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_ArrayLOG10Tag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_ArrayEXPTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_ArrayEXPTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_ArraySQRTTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_ArraySQRTTag " << notImplemented << endl;
            // printf ("Found an ARRAY_FUNC_SQRT_OP \n");

               ROSE_ArraySQRT* arraySqrtOperator = new ROSE_ArraySQRT (expr);
	       ROSE_ASSERT (arraySqrtOperator != NULL);

            // error recover mechanism
               if (arraySqrtOperator->error())
                  {
                 // We have detected an error found in parsing the Sage tree to build the ROSE_ArrayParenthesisOperator
                    returnNode = NULL;
                  }
                 else
                  {
                 // Build a reference to the expression list
                    SgExprListExp* sageExprListExp = arraySqrtOperator->getSageExpressionListExpression();

                    SgExpressionPtrList & expressionPtrList = sageExprListExp->get_expressions();
                    SgExpressionPtrList::iterator i = expressionPtrList.begin();
                    if (i == expressionPtrList.end())
                       {
                      // cout << "<EMPTY LIST>" << endl;
                       }
                      else
                       {
                         int counter = 0;
                         while (i != expressionPtrList.end()) 
                            {
#if USE_SAGE3
                              SgExpression* expression = *i;
#else
                              SgExpressionPtr expressionPtr = *i;
                              SgExpression* expression = expressionPtr.operator->();
#endif
                           // addArgumentExpression (expression);
                              ROSE_Expression* roseExpression = parse ( expression );
                              ROSE_ASSERT (roseExpression != NULL);
                              arraySqrtOperator->addArgumentExpression (roseExpression);
                              i++;
                              counter++;  // We really don't need this counter
                            }
                       }

                 // printf ("number of arguments = %d \n",arraySqrtOperator->numberOfArguments());
                    returnNode = arraySqrtOperator;
                  }
               break;
             }

          case ROSE_ArrayFABSTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_ArrayFABSTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_ArrayCEILTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_ArrayCEILTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_ArrayFLOORTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_ArrayFLOORTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_ArrayABSTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_ArrayABSTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_ArrayTRANSPOSETag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_ArrayTRANSPOSETag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_ArrayACOSTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_ArrayACOSTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_ArrayASINTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_ArrayASINTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_ArrayATANTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_ArrayATANTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_ArrayCOSHTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_ArrayCOSHTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_ArraySINHTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_ArraySINHTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_ArrayTANHTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_ArrayTANHTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_ArrayACOSHTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_ArrayACOSHTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_ArrayASINHTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_ArrayASINHTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_ArrayATANHTag: 
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_ArrayATANHTag " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
          case ROSE_LAST_TAG:
#if 0
             {
               if (ROSE_VALIDATION_AND_TRANSFORMATION_DEBUG > 0) cout << "ROSE_LAST_TAG " << notImplemented << endl;
	       ROSE_ABORT();
               break;
             }
#endif
          case ROSE_UNKNOWN_GRAMMAR:
             {
            // If it is an unknown part of the array grammar then assume it is a C_Expression
            // The difference between this case and that of the ARRAY_SCALAR_C_EXPRESSION 
            // is that this case will not continue to parse
               if ( (ROSE_DEBUG > 0) || (ROSE::verbose == TRUE) )
                    printf ("WARNING NOT AN ARRAY EXPRESSION: ARRAY_FUNC_UNKNOWN_OP in switch! \n");
            // ROSE_ABORT();

            // Build a C_Expression in this case
               ROSE_C_Expression* arrayCExpression = new ROSE_C_Expression (expr);
	       ROSE_ASSERT (arrayCExpression != NULL);

            // error recover mechanism
               if (arrayCExpression->error())
                  {
                 // We have detected an error found in parsing the Sage tree to build the ROSE_ArrayParenthesisOperator
                    returnNode = NULL;
                  }
                 else
                  {
                    returnNode = arrayCExpression;
                  }
               break;
             }
          default:
               printf ("Default reached in parse(SgExpression*) arrayClassVariant = %d \n",arrayClassVariant);
	       ROSE_ABORT();
        }

     ROSE_ASSERT (returnNode != NULL);
     return returnNode;
   }




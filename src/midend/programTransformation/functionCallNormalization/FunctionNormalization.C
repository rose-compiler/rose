// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "FunctionNormalization.h"

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

/*
 * Normalization of function calls - no statement will have more than one function call
 * This is to be done by inserting new variables, while preserving the semantics of the code.
 * Author: Radu Popovici
 * Date: Sept-08-2005
 */

/*
Example:
Assume we have the functions:
  bool g ( int );
  int f1 ( bool );
  int &f2 ( bool );
and the code:
  for ( ; f1( g( 1 ) ) < 10; f2( g( 2 ) ) );

The code will become ( note the three different types of statements generated ):
  bool temp1 = g( 1 );        // --- type1
  int temp2 = f1( temp1 );    // --- type1
  bool temp3;                 // --- type2
  int *temp4;                 // --- type2
  for ( ; temp2 < 10; *temp4 )
  {
    temp3 = g( 2 );           // --- type3
    temp4 = &f2( temp3 );     // --- type3
    temp1 = g( 1 );           // --- type3
    temp2 = f1( temp1 );      // --- type3
  }

We generated 3 types of statements: initialized variable declaration, non-initialized variable declaration and assignment.
For each statement we process we will generate a list structures, one structure for each function call of the statement.
The structure is called Declarations, and contains the three types of statements for the desired function call
Depending on the enclosing scope we will insert the appropriate type(s) of statements to replace each function call.

Note: in the case of functions returning references, we need to assign those to pointers, in the case when we cannot
initialize the substituting variable at the point of its declaration (e.g. for the increment of for-loops).

The Algorithm:
1. get statement
2. generate list of (direct) function calls in the statement (not considering statements associated with the current one)
3. for each function call, generate a structure containing the 3 statements, one of each type (some may be null in some cases)
4. insert the newly created statements where appropriate
5. replace in the original expression the function calls with their corresponding variables (at the highest level)
*/

void
FunctionCallNormalization::visit( SgNode *astNode )
   {
     SgStatement *stm = isSgStatement( astNode );

     // visiting all statements which may contain function calls;
     // Note 1: we do not look at the body of loops, or sequences of statements, but only
     // at statements which may contain directly function calls; all other statements will have their component parts visited in turn
     if ( isSgEnumDeclaration( astNode ) || isSgVariableDeclaration( astNode ) || isSgVariableDefinition( astNode ) ||
                               isSgExprStatement( astNode ) || isSgForStatement( astNode ) || isSgReturnStmt( astNode ) ||
                               isSgSwitchStatement( astNode ) )
        {
       // maintain the mappings from function calls to expressions (variables or dereferenced variables)
          map<SgFunctionCallExp *, SgExpression *> fct2Var;

       // list of Declaration structures, one structure per function call
          DeclarationPtrList declarations;
          bool variablesDefined = false;
             
       // list of function calls, in correnspondence with the inForTest list below
          list<SgNode*> functionCallExpList;
          list<bool> inForTest;

          SgForStatement *forStm = isSgForStatement( stm );
          SgSwitchStatement *swStm = isSgSwitchStatement( stm );
          list<SgNode*> temp1, temp2;

       // for-loops and Switch statements have conditions ( and increment ) expressed as expressions
       // and not as standalone statements; this will change in future Sage versions
       // TODO: when for-loops and switch statements have conditions expressed via SgStatements
       // these cases won't be treated separately; however, do-while will have condition expressed via expression
       // so that will be the only exceptional case to be treated separately
          if (forStm != NULL)
             {
            // create a list of function calls in the condition and increment expression
            // the order is important, the condition is evaluated after the increment expression
            // temp1 = FEOQueryForNodes( forStm->get_increment_expr_root(), V_SgFunctionCallExp );
            // temp2 = FEOQueryForNodes( forStm->get_test_expr_root(), V_SgFunctionCallExp );
               temp1 = FEOQueryForNodes( forStm->get_increment(), V_SgFunctionCallExp );
               temp2 = FEOQueryForNodes( forStm->get_test_expr(), V_SgFunctionCallExp );
               functionCallExpList = temp1;
               functionCallExpList.splice( functionCallExpList.end(), temp2 );
             }
            else
             {
               if (swStm != NULL)
                  {
                 // create a list of function calls in the condition in the order of function evaluation
                 // DQ (11/23/2005): Fixed SgSwitchStmt to have SgStatement for conditional.
                 // list<SgNode*> temp1 = FEOQueryForNodes( swStm->get_item_selector_root(), V_SgFunctionCallExp );
                    list<SgNode*> temp1 = FEOQueryForNodes( swStm->get_item_selector(), V_SgFunctionCallExp );
                    functionCallExpList = temp1;
                  }
                 else
                  {
                 // create a list of function calls in the statement in the order of function evaluation
                    functionCallExpList = FEOQueryForNodes( stm, V_SgFunctionCallExp );
                  }
             }

         // all function calls get replaced: this is because they can occur in expressions (e.g. for-loops)
         // which makes it difficult to build control flow graphs
         if ( functionCallExpList.size() > 0 )
           {
             cout << "--------------------------------------\nStatement ";
             cout << stm->unparseToString() << "\n";;
             
             // traverse the list of function calls in the current statement, generate a structure  Declaration for each call
             // put these structures in a list to be inserted in the code later
             for ( list<SgNode *>::iterator i = functionCallExpList.begin(); i != functionCallExpList.end(); i++ )
               {
                 variablesDefined = true;

                 // get function call exp
                 SgFunctionCallExp *exp = isSgFunctionCallExp( *i );
                 ROSE_ASSERT ( exp );
                 
                 // get type of expression, generate unique variable name
                 SgType *expType = exp->get_type();
                 ROSE_ASSERT ( expType );
                 Sg_File_Info *location = Sg_File_Info::generateDefaultFileInfoForTransformationNode();
                 ROSE_ASSERT ( location );
                 ostringstream os;
                 os << "__tempVar__" << location;
                 SgName name = os.str().c_str();

                 // replace previous variable bindings in the AST
                 SgExprListExp *paramsList = exp->get_args();
                 SgExpression *function = exp->get_function();
                 ROSE_ASSERT ( paramsList && function );
                 replaceFunctionCallsInExpression( paramsList, fct2Var );
                 replaceFunctionCallsInExpression( function, fct2Var );

                 // duplicate function call expression, for the initialization declaration and the assignment
                 SgTreeCopy treeCopy;
                 SgFunctionCallExp *newExpInit = isSgFunctionCallExp( exp->copy( treeCopy ) );
                 ROSE_ASSERT ( newExpInit );
                 SgFunctionCallExp *newExpAssign = isSgFunctionCallExp( exp->copy( treeCopy ) );
                 ROSE_ASSERT ( newExpAssign );

                 // variables
                 Sg_File_Info *initLoc = Sg_File_Info::generateDefaultFileInfoForTransformationNode(),
                   *nonInitLoc = Sg_File_Info::generateDefaultFileInfoForTransformationNode(),
                   *assignLoc = Sg_File_Info::generateDefaultFileInfoForTransformationNode();
                 Declaration *newDecl = new Declaration();
                 SgStatement *nonInitVarDeclaration, *initVarDeclaration, *assignStmt;
                 SgExpression *varRefExp;
                 SgVariableSymbol *varSymbol;
                 SgAssignOp *assignOp;
                 SgInitializedName *initName;

                 bool pointerTypeNeeded = false;

                 // mark whether to replace inside or outside of ForStatement due to the
                 // function call being inside the test or the increment for a for-loop statement
                 // the 'inForTest' list is in 1:1  ordered correpondence with the 'declarations' list
                 if ( forStm )
                   {
        // SgExpressionRoot
                  //   *testExp = isSgForStatement( astNode )->get_test_expr_root(),
                  //   *incrExp = isSgForStatement( astNode )->get_increment_expr_root();
                     SgExpression
                       *testExp = isSgForStatement( astNode )->get_test_expr(),
                       *incrExp = isSgForStatement( astNode )->get_increment();
                     SgNode *up = exp;
                     while ( up && up != testExp && up != incrExp )
                       up = up->get_parent();
                     ROSE_ASSERT ( up );

                     // function call is in the condition of the for-loop
                     if ( up == testExp )
                       inForTest.push_back( true );
                     // function call is in the increment expression
                     else
                       {
                         inForTest.push_back( false );

                         // for increment expressions we need to be able to reassign the return value
                         // of the function; if the ret value is a reference, we need to generate a
                         // pointer of that type (to be able to reassign it later)
                         if ( isSgReferenceType( expType ) )
                           pointerTypeNeeded = true;
                       }
                   }

                 // for do-while statements:  we need to generate declaration of type pointer to be able to have
                 // non-assigned references when looping and assign them at the end of the body of the loop
                 if ( isSgDoWhileStmt( stm->get_parent() ) && isSgReferenceType( expType ) )
                   pointerTypeNeeded = true;

                 // we have a function call returning a reference and we can't initialize the variable
                 // at the point of declaration; we need to define the variable as a pointer
                 if ( pointerTypeNeeded )
                   {
                     // create 'address of' term for function expression, so we can assign it to the pointer
                     SgAddressOfOp *addressOp = new SgAddressOfOp( assignLoc, newExpAssign, expType );

                     // create noninitialized declaration
                     SgType *base = isSgReferenceType( expType )->get_base_type();
                     ROSE_ASSERT( base );
                     SgPointerType *ptrType = SgPointerType::createType( isSgReferenceType( expType )->get_base_type() );
                     ROSE_ASSERT ( ptrType );
                     nonInitVarDeclaration = new SgVariableDeclaration ( nonInitLoc, name, ptrType );

                     // create assignment (symbol, varRefExp, assignment)
                     initName = isSgVariableDeclaration( nonInitVarDeclaration )->get_decl_item( name );
                     ROSE_ASSERT ( initName );

                     varSymbol = new SgVariableSymbol( initName );
                     ROSE_ASSERT ( varSymbol );
                     varRefExp = new SgVarRefExp( assignLoc, varSymbol );

                     SgPointerDerefExp *ptrDeref= new SgPointerDerefExp( assignLoc, varRefExp, expType );
                     ROSE_ASSERT ( isSgExpression( varRefExp ) && ptrDeref );
                     assignOp = new SgAssignOp( assignLoc, varRefExp, addressOp, ptrType );
                     assignStmt = new SgExprStatement( assignLoc, assignOp );
                     ROSE_ASSERT ( assignStmt &&  nonInitVarDeclaration );
           
                     // we don't need initialized declarations in this case
                     initVarDeclaration = NULL;

                     // save new mapping
                     fct2Var.insert( Fct2Var( exp, ptrDeref ) );
                   }
                 else
                   {
                     // create (non- &)initialized declarations, initialized name & symbol
                     SgAssignInitializer *declInit = new SgAssignInitializer( initLoc, newExpInit, expType );
                     ROSE_ASSERT ( declInit );
                     initVarDeclaration = new SgVariableDeclaration ( initLoc, name, expType, declInit );
                     nonInitVarDeclaration = new SgVariableDeclaration ( nonInitLoc, name, expType );
                     ROSE_ASSERT ( initVarDeclaration && nonInitVarDeclaration );

                     initName = isSgVariableDeclaration( nonInitVarDeclaration )->get_decl_item( name );
                     ROSE_ASSERT ( initName );
                     newExpInit->set_parent( initName );
                     varSymbol = new SgVariableSymbol( initName );
                     ROSE_ASSERT ( varSymbol );

                     // create variable ref exp
                     varRefExp = new SgVarRefExp( assignLoc, varSymbol );
                     ROSE_ASSERT ( isSgVarRefExp( varRefExp ) );

                     // create the assignment
                     assignOp = new SgAssignOp( assignLoc, varRefExp, newExpAssign, expType );
                     assignStmt = new SgExprStatement( assignLoc, assignOp );
                     ROSE_ASSERT ( assignStmt );

                     initVarDeclaration->set_parent( stm->get_parent() );
                     isSgVariableDeclaration( initVarDeclaration )->set_definingDeclaration( isSgDeclarationStatement( initVarDeclaration ) );

                     // save new mapping
                     fct2Var.insert( Fct2Var( exp, varRefExp ) );
                   }

                 // save the 'declaration' structure, with all 3 statements and the variable name
                 newDecl->nonInitVarDeclaration = nonInitVarDeclaration;
                 newDecl->initVarDeclaration = initVarDeclaration;
                 newDecl->assignment = assignStmt;
                 newDecl->name = name;
                 nonInitVarDeclaration->set_parent( stm->get_parent() );
                 isSgVariableDeclaration( nonInitVarDeclaration )->set_definingDeclaration( isSgVariableDeclaration( nonInitVarDeclaration ) );
                 assignStmt->set_parent( stm->get_parent() );
                 declarations.push_back( newDecl );
               } // end for
           } // end if  fct calls in crt stmt > 1

         SgScopeStatement *scope = stm->get_scope();
         ROSE_ASSERT ( scope );
         
         // insert function bindings to variables; each 'declaration' structure in the list
         // corresponds to one function call
         for ( DeclarationPtrList::iterator i = declarations.begin(); i != declarations.end(); i++ )
           {
             Declaration *d = *i;
             ROSE_ASSERT ( d && d->assignment && d->nonInitVarDeclaration );

             // if the current statement is a for-loop, we insert Declarations before & in the loop body, depending on the case
             if ( forStm )
               {
                 SgStatement *parentScope = isSgStatement( stm->get_scope() );
                 SgBasicBlock *body = SageInterface::ensureBasicBlockAsBodyOfFor(forStm);
                 ROSE_ASSERT ( !inForTest.empty() && body && parentScope );
                 // SgStatementPtrList &list = body->get_statements();

                 // if function call is in loop condition, we add initialized variable before the loop and at its end
                 // hoist initialized variable declarations outside the loop
                 if ( inForTest.front() )
                   {
                     ROSE_ASSERT ( d->initVarDeclaration );
                     parentScope->insert_statement( stm, d->initVarDeclaration );

                     // set the scope of the initializedName
                     SgInitializedName *initName = isSgVariableDeclaration( d->initVarDeclaration )->get_decl_item( d->name );
                     ROSE_ASSERT ( initName );
                     initName->set_scope( isSgScopeStatement( parentScope ) );
                     ROSE_ASSERT ( initName->get_scope() );
                   }
                 // function call is in loop post increment so add noninitialized variable decls above the loop
                 else
                   {
                     parentScope->insert_statement( stm, d->nonInitVarDeclaration );

                     // set the scope of the initializedName
                     SgInitializedName *initName = isSgVariableDeclaration( d->nonInitVarDeclaration )->get_decl_item( d->name );
                     ROSE_ASSERT ( initName );
                     initName->set_scope( isSgScopeStatement( parentScope ) );
                     ROSE_ASSERT ( initName->get_scope() );
                   }

                 // in a for-loop, always insert assignments at the end of the loop
                 body->get_statements().push_back( d->assignment );
                 d->assignment->set_parent( body );

                 // remove marker
                 inForTest.pop_front();
               }
             else
               {
                 // look at the type of the enclosing scope
                 switch ( scope->variantT() )
                   {

                     // while stmts have to repeat the function calls at the end of the loop;
                     // note there is no "break" statement, since we want to also add initialized
                     // declarations before the while-loop
                   case V_SgWhileStmt:
                     {
                       // assignments need to be inserted at the end of each while loop
                       SgBasicBlock *body = SageInterface::ensureBasicBlockAsBodyOfWhile(isSgWhileStmt( scope ) );
                       ROSE_ASSERT ( body );
                       d->assignment->set_parent( body );
                       body->get_statements().push_back( d->assignment );
                     }

                     // SgForInitStatement has scope SgForStatement, move declarations before the for loop;
                     // same thing if the enclosing scope is an If, or Switch statement
                   case V_SgForStatement:
                   case V_SgIfStmt:
                   case V_SgSwitchStatement:
                     {
                       // adding bindings (initialized variable declarations only, not assignments)
                       // outside the statement, in the parent scope
                       SgStatement *parentScope = isSgStatement( scope->get_parent() );
                       ROSE_ASSERT ( parentScope );
                       parentScope->insert_statement( scope, d->initVarDeclaration, true );\

                       // setting the scope of the initializedName
                       SgInitializedName *initName = isSgVariableDeclaration( d->initVarDeclaration )->get_decl_item( d->name );
                       ROSE_ASSERT ( initName );
                       initName->set_scope( scope->get_scope() );
                       ROSE_ASSERT ( initName->get_scope() );
                     }
                     break;

                     // do-while needs noninitialized declarations before the loop, with assignments inside the loop
                   case V_SgDoWhileStmt:
                     {
                       // adding noninitialized variable declarations before the body of the loop
                       SgStatement *parentScope = isSgStatement( scope->get_parent() );
                       ROSE_ASSERT ( parentScope );
                       parentScope->insert_statement( scope, d->nonInitVarDeclaration, true );

                       // initialized name scope setting
                       SgInitializedName *initName = isSgVariableDeclaration( d->nonInitVarDeclaration )->get_decl_item( d->name );
                       ROSE_ASSERT ( initName );
                       initName->set_scope( scope->get_scope() );
                       ROSE_ASSERT ( initName->get_scope() );

                       // adding assignemts at the end of the do-while loop
                       SgBasicBlock *body = SageInterface::ensureBasicBlockAsBodyOfDoWhile( isSgDoWhileStmt(scope) );
                       ROSE_ASSERT ( body );
                       body->get_statements().push_back( d->assignment );
                       d->assignment->set_parent(body);
                     }
                     break;

                     // for all other scopes, add bindings ( initialized declarations ) before the statement, in the same scope
                   default:
                     scope->insert_statement( stm, d->initVarDeclaration, true );

                     // initialized name scope setting
                     SgInitializedName *initName = isSgVariableDeclaration( d->initVarDeclaration )->get_decl_item( d->name );
                     ROSE_ASSERT ( initName );
                     initName->set_scope( scope->get_scope() );
                     ROSE_ASSERT ( initName->get_scope() );
                   }
               }
           }
         
         // once we have inserted all variable declarations, we need to replace top-level calls in the original statement
         if ( variablesDefined )
           {
             cout << "\tReplacing in the expression " << stm->unparseToString() << "\n";

             // for ForStatements, replace expressions in condition and increment expressions,
             // not in the body, since those get replace later
             if ( forStm )
               {
         // SgExpressionRoot *testExp = forStm->get_test_expr_root(), *incrExp = forStm->get_increment_expr_root();
            SgExpression *testExp = forStm->get_test_expr(), *incrExp = forStm->get_increment();
            replaceFunctionCallsInExpression( incrExp, fct2Var );
            replaceFunctionCallsInExpression( testExp, fct2Var );
               }
             else
               if ( swStm )
             {
            // DQ (11/23/2005): Fixed SgSwitch to permit use of declaration for conditional
            // replaceFunctionCallsInExpression( swStm->get_item_selector_root(), fct2Var );
               replaceFunctionCallsInExpression( swStm->get_item_selector(), fct2Var );
             }
               else
             replaceFunctionCallsInExpression( stm, fct2Var );
           }
       } // end if isSgStatement block
   }

/*
Given a node in the AST, it replaces all function calls with the corresponding expression from the fct2Var mapping.
Since we traverse in order of function evaluation, all nodes that are lower in the subtree are supposed to
have been mapped, since they have already appeared in the list. Also, we only replace the shallowest level of
function calls in the surrent subtree, since we assume that replaceFunctionCallsInExpression  has already been called
for each of the subtrees rooted at the shallowest function calls lower than the current node:

E.g.: f(g(h(i))) + 5
has the AST:
     +
    / \
  fc1  5
  / \
 f  fc2
    / \
   g  fc3 
      / \
     h   i
where fc? represents an SgFunctionCallExpression. Order of function evaluation is h(i), g(_), f(_).

Calling 'replaceFunctionCallsInExpression' on '+' will generate
                     temp_var + 5
but calling the same function on 'fc2' will generate
                     f(temp_var) + 5
*/
void
FunctionCallNormalization::replaceFunctionCallsInExpression( SgNode *root,
                                                             map<SgFunctionCallExp *, SgExpression *> fct2Var )
   {
     if ( !root )
       return;

     if ( NodeQuery::querySubTree( root, V_SgFunctionCallExp ).size() > 0 )
       {
         list<SgNode *> toVisit;
         toVisit.push_back( root );

         while ( !toVisit.empty() )
           {
             SgNode *crt = toVisit.front();

             // will visit every node above an function call exp, but not below
             // also, we will replace function call expressions found
             if ( !isSgFunctionCallExp( crt ) )
               {
                 vector<SgNode *> succ = ( crt )->get_traversalSuccessorContainer();
                 for ( vector<SgNode *>::iterator succIt = succ.begin(); succIt != succ.end(); succIt++ )
                   if ( isSgNode ( *succIt ) )
                     toVisit.push_back( *succIt );
               }
             else
               {
                 SgFunctionCallExp *call = isSgFunctionCallExp( crt );
                 ROSE_ASSERT ( call );
                 
                 map<SgFunctionCallExp *, SgExpression *>::iterator mapIter;
                 mapIter = fct2Var.find( call );
                 if ( mapIter == fct2Var.end() )
                   cout << "NOT FOUND " << call->unparseToString() << "\t" << call << "\n";

                 // a mapping for function call exps in the subtree must already exist ( postorder traversal )
                 ROSE_ASSERT ( mapIter != fct2Var.end() && mapIter->second );

                 // duplicate the variable
                 SgTreeCopy treeCopy;
                 SgExpression *scnd = mapIter->second;
                 ROSE_ASSERT ( isSgExpression( scnd ) );
                 SgExpression *newVar = isSgExpression( scnd->copy( treeCopy ) );
                 ROSE_ASSERT ( newVar );

                 SgExpression *parent = isSgExpression( call->get_parent() );
                 ROSE_ASSERT ( parent );

                 // replace the node in the AST
                 newVar->set_parent( parent );
                 int k = parent->replace_expression( call, newVar );
                 ROSE_ASSERT ( k == 1 );
                 delete call;
               }
             toVisit.pop_front();
           }
       }
   }

/*
Query on a the AST using the VariantT and BFS traversal
*/
list<SgNode *>
FunctionCallNormalization::BFSQueryForNodes( SgNode *root, VariantT type )
{
  list<SgNode *> toVisit, retList;
  toVisit.push_back( root );

  while ( !toVisit.empty() )
    {
      SgNode *crt = toVisit.front();
      if ( crt->variantT() == type )
        retList.push_back( crt );
      
      vector<SgNode *> succ = ( crt )->get_traversalSuccessorContainer();
      for ( vector<SgNode *>::iterator succIt = succ.begin(); succIt != succ.end(); succIt++ )
        if ( isSgNode ( *succIt ) )
          toVisit.push_back( *succIt );
      
      toVisit.pop_front();
    }
  return retList;
}

/*
Query on a list of nodes using the VariantT
*/
list<SgNode *>
FunctionCallNormalization::FEOQueryForNodes( SgNode *root, VariantT type )
   {
     list<SgNode *> toVisit = createTraversalList( root );
     list<SgNode *> retList;

     for ( list<SgNode *>::iterator succIt = toVisit.begin(); succIt != toVisit.end(); succIt++ )
       if ( isSgNode( *succIt ) && isSgNode( *succIt )->variantT() == type )
         {
           retList.push_back( *succIt );
           cout << "Function " << isSgNode( *succIt )->unparseToString() << "\t" << *succIt << "\n";
         }

     return retList;
   }

/*
Creates a list of nodes in Function evaluation order (FEO) (first eval function expression, then args; for other nodes, it's BFS)
For the previous example, if f, g, and h were expressions (pointers) evaluating to functions, the order is:
f, g, h, h(i), g(_), f(_).
Note: This is not always equivalent to postorder (because of the BFS on nodes other than function calls).
*/
list<SgNode *>
FunctionCallNormalization::createTraversalList( SgNode *root )
   {
     list<SgNode *> retList;

     if ( isSgFunctionCallExp( root ) )
       {
         list<SgNode *> temp1 = createTraversalList( isSgFunctionCallExp( root )->get_function() );
         list<SgNode *> temp2 = createTraversalList( isSgFunctionCallExp( root )->get_args() );
         retList = temp1;
         retList.splice( retList.end(), temp2 );
         retList.push_back( root );
       }
     else
       if ( isSgNode( root ) )
         {
           vector<SgNode *> succ = root->get_traversalSuccessorContainer();
           for ( vector<SgNode *>::iterator succIt = succ.begin(); succIt != succ.end(); succIt++ )
             if ( isSgNode ( *succIt ) )
               {
                 list<SgNode *> temp1 = createTraversalList( *succIt );
                 retList.splice( retList.end(), temp1 );
               }
           retList.push_back( root );
         }
     return retList;       
   }

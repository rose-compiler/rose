
// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "AstConsistencyTests.h"
#include "unparser.h"

#include "inlinerSupport.h"
#include "replaceExpressionWithStatement.h"
#include "expressionTreeEqual.h"
#include <vector>
#include <iostream>
#include <iomanip>
#include "patternRewrite.h"
#include "pre.h"

#undef FD_DEBUG

// DQ (8/1/2005): test use of new static function to create 
// Sg_File_Info object that are marked as transformations
#undef SgNULL_FILE
#define SgNULL_FILE Sg_File_Info::generateDefaultFileInfoForTransformationNode()

using namespace std;

void FixSgTree(SgNode*);

class FdFindCopiesVisitor: public AstSimpleProcessing {
  SgExpression* target;
  vector<SgExpression*>& copies;

  public:
  FdFindCopiesVisitor(SgExpression* target, vector<SgExpression*>& copies):
      target(target), copies(copies) {
#ifdef FD_DEBUG
    cout << "Looking for copies of " << target->unparseToString() << endl;
#endif
  }

  virtual void visit(SgNode* n) {
#ifdef FD_DEBUG
    cout << "FdFindCopiesVisitor visiting" << n->sage_class_name() << endl;
#endif
    if (isSgExpression(n) && expressionTreeEqual(isSgExpression(n), target)) {
#ifdef FD_DEBUG
      cout << "Found copy " << n->unparseToString() << endl;
#endif
      copies.push_back(isSgExpression(n));
    }
  }
};

void replaceCopiesOfExpression(SgExpression* src, SgExpression* tgt, 
                               SgNode* root) {
#ifdef FD_DEBUG
  cout << "replaceCopiesOfExpression: src = " << src->unparseToString() << ", tgt = " << tgt->unparseToString() << ", root = " << root->unparseToString() << endl;
#endif
  vector<SgExpression*> copies_of_src;
#ifdef FD_DEBUG
  cout << "---" << endl;
#endif
  FdFindCopiesVisitor(src, copies_of_src).traverse(root, preorder);
#ifdef FD_DEBUG
  cout << copies_of_src.size() << " copy(ies) found." << endl;
#endif
  for (unsigned int i = 0; i < copies_of_src.size(); ++i) {
    SgTreeCopy tc;
    SgExpression* copy = isSgExpression(tgt->copy(tc));
    SgExpression* parent = isSgExpression(copies_of_src[i]->get_parent());
    assert (parent);
    parent->replace_expression(copies_of_src[i], copy);
    copy->set_parent(parent);
  }
#ifdef FD_DEBUG
  cout << "result is " << root->unparseToString() << endl;
#endif
}

class FdFindModifyingStatementsVisitor
   : public AstSimpleProcessing
   {
     const vector<SgVariableSymbol*> & syms;
     vector<SgExpression*> &           mods;
     vector<SgAssignInitializer*>      initializersToSplit;

     public:
          FdFindModifyingStatementsVisitor(const vector<SgVariableSymbol*>& syms, vector<SgExpression*>& mods)
             : syms(syms), mods(mods) {}

          void go(SgNode* root)
             {
            // Not thread safe or reentrant
               initializersToSplit.clear();
               traverse(root, postorder);
               for (unsigned int i = 0; i < initializersToSplit.size(); ++i)
                  {
                    SgAssignOp* assignment = convertInitializerIntoAssignment(initializersToSplit[i]);
                    assert(assignment->variantT() == V_SgAssignOp);
                    mods.push_back(assignment);
                  }
             }

  virtual void visit(SgNode* n) {
    switch (n->variantT()) {
      case V_SgAssignOp:
      case V_SgPlusAssignOp:
      case V_SgMinusAssignOp:
      case V_SgAndAssignOp:
      case V_SgIorAssignOp:
      case V_SgMultAssignOp:
      case V_SgDivAssignOp:
      case V_SgModAssignOp:
      case V_SgXorAssignOp:
      case V_SgLshiftAssignOp:
      case V_SgRshiftAssignOp:
      if (anyOfListPotentiallyModifiedIn(syms, n))
        mods.push_back(isSgExpression(n));
      break;

      case V_SgPlusPlusOp:
      case V_SgMinusMinusOp:
      if (anyOfListPotentiallyModifiedIn(syms, n))
        mods.push_back(isSgExpression(n));
      break;

      case V_SgAssignInitializer: {
        SgAssignInitializer* init = isSgAssignInitializer(n);
        assert (init);
        for (unsigned int i = 0; i < syms.size(); ++i) {
          SgInitializedName* initname = syms[i]->get_declaration();
          if (init->get_parent() == initname ||
              init->get_parent()->get_parent() == initname) {
            initializersToSplit.push_back(init);
            break;
          }
        }
      }
      break;

      default: break;
    }
  }
};

SgExpression* doFdVariableUpdate(
    RewriteRule* rules,
    SgExpression* cache,
    SgExpression* old_val /* Cannot be referenced in output tree w/o copying */,
    SgExpression* new_val)
   {
#ifdef FD_DEBUG
  cout << "Trying to convert from " << old_val->unparseToString() << " to " << new_val->unparseToString() << ", using cache " << cache->unparseToString() << endl;
#endif
     SgTreeCopy tc;
     SgExpression* old_valCopy = isSgExpression(old_val->copy(tc));
     ROSE_ASSERT (old_valCopy);
     SgCommaOpExp* innerComma = new SgCommaOpExp(SgNULL_FILE, old_valCopy, new_val);
     old_valCopy->set_parent(innerComma);
     new_val->set_parent(innerComma);
     SgExpression* expr = new SgCommaOpExp(SgNULL_FILE,cache,innerComma);
     cache->set_parent(expr);
     innerComma->set_parent(expr);
     // This is done so rewrite's expression replacement code will never find a
     // NULL parent for the expression being replaced
     SgExprStatement* dummyExprStatement =
       new SgExprStatement(SgNULL_FILE, expr);
     expr->set_parent(dummyExprStatement);
     SgNode* exprCopyForRewrite = expr;
     rewrite(rules, exprCopyForRewrite); // This might modify exprCopyForRewrite
     ROSE_ASSERT (isSgExpression(exprCopyForRewrite));
     expr = isSgExpression(exprCopyForRewrite);
     expr->set_parent(NULL);
     dummyExprStatement->set_expression(NULL);
     delete dummyExprStatement;
     SgExpression* expr2 = expr;
     ROSE_ASSERT (expr2);
     if (// The rewrite rules may have changed the form of expr to something other than a comma pair
        isSgCommaOpExp(expr2) &&
        isSgCommaOpExp(isSgCommaOpExp(expr2)->get_rhs_operand()))
        {
          SgExpression* cache2 = isSgCommaOpExp(expr2)->get_lhs_operand();
          SgCommaOpExp* rhs = isSgCommaOpExp(isSgCommaOpExp(expr2)->get_rhs_operand());
       // SgExpression* old_val2 = rhs->get_lhs_operand();
          SgExpression* new_val2 = rhs->get_rhs_operand();
       // return new SgAssignOp(SgNULL_FILE, cache2, new_val2);
          cache2->set_lvalue(true);
          SgAssignOp* assignmentOperator = new SgAssignOp(SgNULL_FILE, cache2, new_val2);
          cache2->set_parent(assignmentOperator);
          new_val2->set_parent(assignmentOperator);

#ifdef FD_DEBUG
          printf ("In doFdVariableUpdate(): assignmentOperator = %p \n",assignmentOperator);
#endif

          return assignmentOperator;
        }
       else
        {
          return expr2;
        }
   }

// Do finite differencing on one expression within one context.  The expression
// must be defined and valid within the entire body of root.  The rewrite rules
// are used to simplify expressions.  When a variable var is updated from
// old_value to new_value, an expression of the form (var, (old_value,
// new_value)) is created and rewritten.  The rewrite rules may either produce
// an arbitrary expression (which will be used as-is) or one of the form (var,
// (something, value)) (which will be changed to (var = value)).
void doFiniteDifferencingOne(SgExpression* e, 
                             SgBasicBlock* root,
                             RewriteRule* rules)
   {
     SgStatementPtrList& root_stmts = root->get_statements();
     SgStatementPtrList::iterator i;
     for (i = root_stmts.begin(); i != root_stmts.end(); ++i)
        {
          if (expressionComputedIn(e, *i))
               break;
        }
     if (i == root_stmts.end())
          return; // Expression is not used within root, so quit
     vector<SgVariableSymbol*> used_symbols = SageInterface::getSymbolsUsedInExpression(e);
     SgName cachename = "cache_fd__"; cachename << ++SageInterface::gensym_counter;
     SgVariableDeclaration* cachedecl = new SgVariableDeclaration(SgNULL_FILE, cachename, e->get_type(),0 /* new SgAssignInitializer(SgNULL_FILE, e) */);
     SgInitializedName* cachevar = cachedecl->get_variables().back();
     ROSE_ASSERT (cachevar);
     root->get_statements().insert(i, cachedecl);
     cachedecl->set_parent(root);
     cachedecl->set_definingDeclaration(cachedecl);
     cachevar->set_scope(root);
     SgVariableSymbol* sym = new SgVariableSymbol(cachevar);
     root->insert_symbol(cachename, sym);
     SgVarRefExp* vr = new SgVarRefExp(SgNULL_FILE, sym);
     vr->set_endOfConstruct(SgNULL_FILE);
     replaceCopiesOfExpression(e, vr, root);

     vector<SgExpression*> modifications_to_used_symbols;
     FdFindModifyingStatementsVisitor(used_symbols, modifications_to_used_symbols).go(root);

     cachedecl->addToAttachedPreprocessingInfo( 
          new PreprocessingInfo(PreprocessingInfo::CplusplusStyleComment,(string("// Finite differencing: ") + 
               cachename.str() + " is a cache of " + 
               e->unparseToString()).c_str(),"Compiler-Generated in Finite Differencing",0, 0, 0, PreprocessingInfo::before));

     if (modifications_to_used_symbols.size() == 0)
        {
          SgInitializer* cacheinit = new SgAssignInitializer(SgNULL_FILE, e);
          e->set_parent(cacheinit);
          cachevar->set_initializer(cacheinit);
          cacheinit->set_parent(cachevar);
        }
       else
        {
          for (unsigned int i = 0; i < modifications_to_used_symbols.size(); ++i)
             {
               SgExpression* modstmt = modifications_to_used_symbols[i];
#ifdef FD_DEBUG
               cout << "Updating cache after " << modstmt->unparseToString() << endl;
#endif
               SgExpression* updateCache = 0;
               SgVarRefExp* varref = new SgVarRefExp(SgNULL_FILE, sym);
               varref->set_endOfConstruct(SgNULL_FILE);
               SgTreeCopy tc;
               SgExpression* eCopy = isSgExpression(e->copy(tc));
               switch (modstmt->variantT())
                  {
                    case V_SgAssignOp:
                       {
                         SgAssignOp* assignment = isSgAssignOp(modstmt);
                         assert (assignment);
                         SgExpression* lhs = assignment->get_lhs_operand();
                         SgExpression* rhs = assignment->get_rhs_operand();
                         replaceCopiesOfExpression(lhs, rhs, eCopy);
                       }
                    break;

                    case V_SgPlusAssignOp:
                    case V_SgMinusAssignOp:
                    case V_SgAndAssignOp:
                    case V_SgIorAssignOp:
                    case V_SgMultAssignOp:
                    case V_SgDivAssignOp:
                    case V_SgModAssignOp:
                    case V_SgXorAssignOp:
                    case V_SgLshiftAssignOp:
                    case V_SgRshiftAssignOp:
                       {
                         SgBinaryOp* assignment = isSgBinaryOp(modstmt);
                         assert (assignment);
                         SgExpression* lhs = assignment->get_lhs_operand();
                         SgExpression* rhs = assignment->get_rhs_operand();
                         SgTreeCopy tc;
                         SgExpression* rhsCopy = isSgExpression(rhs->copy(tc));
                         SgExpression* newval = 0;
                         switch (modstmt->variantT())
                            {
#define DO_OP(op, nonassignment) \
                              case V_##op: { \
                                   newval = new nonassignment(SgNULL_FILE, lhs, rhsCopy); \
                                   newval->set_endOfConstruct(SgNULL_FILE); \
                              } \
                              break

                              DO_OP(SgPlusAssignOp, SgAddOp);
                              DO_OP(SgMinusAssignOp, SgSubtractOp);
                              DO_OP(SgAndAssignOp, SgBitAndOp);
                              DO_OP(SgIorAssignOp, SgBitOrOp);
                              DO_OP(SgMultAssignOp, SgMultiplyOp);
                              DO_OP(SgDivAssignOp, SgDivideOp);
                              DO_OP(SgModAssignOp, SgModOp);
                              DO_OP(SgXorAssignOp, SgBitXorOp);
                              DO_OP(SgLshiftAssignOp, SgLshiftOp);
                              DO_OP(SgRshiftAssignOp, SgRshiftOp);
#undef DO_OP

                              default: break;
                            }
                         assert (newval);
                         replaceCopiesOfExpression(lhs, newval, eCopy);
                       }
                    break;

                    case V_SgPlusPlusOp:
                       {
                         SgExpression* lhs = isSgPlusPlusOp(modstmt)->get_operand();
                         SgIntVal* one = new SgIntVal(SgNULL_FILE, 1);
                         one->set_endOfConstruct(SgNULL_FILE);
                         SgAddOp* add = new SgAddOp(SgNULL_FILE, lhs, one);
                         add->set_endOfConstruct(SgNULL_FILE);
                         lhs->set_parent(add);
                         one->set_parent(add);
                         replaceCopiesOfExpression(lhs,add,eCopy);
                            }
                    break;

                    case V_SgMinusMinusOp:
                       {
                         SgExpression* lhs = isSgMinusMinusOp(modstmt)->get_operand();
                         SgIntVal* one = new SgIntVal(SgNULL_FILE, 1);
                         one->set_endOfConstruct(SgNULL_FILE);
                         SgSubtractOp* sub = new SgSubtractOp(SgNULL_FILE, lhs, one);
                         sub->set_endOfConstruct(SgNULL_FILE);
                         lhs->set_parent(sub);
                         one->set_parent(sub);
                         replaceCopiesOfExpression(lhs,sub,eCopy);
                       }
                    break;

                    default:
                         cerr << modstmt->sage_class_name() << endl;
                         assert (false);
                         break;
                  }

#ifdef FD_DEBUG
            cout << "e is " << e->unparseToString() << endl;
            cout << "eCopy is " << eCopy->unparseToString() << endl;
#endif
               updateCache = doFdVariableUpdate(rules, varref, e, eCopy);
#ifdef FD_DEBUG
            cout << "updateCache is " << updateCache->unparseToString() << endl;
#endif
               if (updateCache)
                  {
                    ROSE_ASSERT(modstmt != NULL);
                    SgNode* ifp = modstmt->get_parent();
                    SgCommaOpExp* comma = new SgCommaOpExp(SgNULL_FILE, updateCache, modstmt);
                    modstmt->set_parent(comma);
                    updateCache->set_parent(comma);

                    if (ifp == NULL)
                       {
                         printf ("modstmt->get_parent() == NULL modstmt = %p = %s \n",modstmt,modstmt->class_name().c_str());
                         modstmt->get_startOfConstruct()->display("modstmt->get_parent() == NULL: debug");
                       }
                    ROSE_ASSERT(ifp != NULL);
#ifdef FD_DEBUG
                 cout << "New expression is " << comma->unparseToString() << endl;
                 cout << "IFP is " << ifp->sage_class_name() << ": " << ifp->unparseToString() << endl;
#endif
                    if (isSgExpression(ifp))
                       {
                         isSgExpression(ifp)->replace_expression(modstmt, comma);
                         comma->set_parent(ifp);
                       }
                      else
                       {
                      // DQ (12/16/2006): Need to handle cases that are not SgExpression (now that SgExpressionRoot is not used!)
                      // cerr << ifp->sage_class_name() << endl;
                      // assert (!"Bad parent type for inserting comma expression");
                         SgStatement* statement = isSgStatement(ifp);
                         if (statement != NULL)
                            {
#ifdef FD_DEBUG
                              printf ("Before statement->replace_expression(): statement = %p = %s modstmt = %p = %s \n",statement,statement->class_name().c_str(),modstmt,modstmt->class_name().c_str());
                              SgExprStatement* expresionStatement = isSgExprStatement(statement);
                              if (expresionStatement != NULL)
                                 {
                                   SgExpression* expression = expresionStatement->get_expression();
                                   printf ("expressionStatement expression = %p = %s \n",expression,expression->class_name().c_str());
                                 }
#endif
                              statement->replace_expression(modstmt, comma);
                              comma->set_parent(statement);
                            }
                           else
                            {
                              ROSE_ASSERT(ifp != NULL);
                              printf ("Error: parent is neither a SgExpression nor a SgStatement ifp = %p = %s \n",ifp,ifp->class_name().c_str());
                              ROSE_ASSERT(false);
                            }
                       }

#ifdef FD_DEBUG
                    cout << "IFP is now " << ifp->unparseToString() << endl;
#endif
                  }
             }
        }
   }

// Propagate definitions of a variable to its uses.
// Assumptions: var is only assigned at the top level of body
//              nothing var depends on is assigned within body
// Very simple algorithm designed to only handle simplest cases
void simpleUndoFiniteDifferencingOne(SgBasicBlock* body, SgExpression* var)
   {
     SgExpression* value = 0;
     SgStatementPtrList& stmts = body->get_statements();
     vector<SgStatement*> stmts_to_remove;

     for (SgStatementPtrList::iterator i = stmts.begin(); i != stmts.end(); ++i)
        {
       // cout << "Next statement: value = " << (value ? value->unparseToString() : "(null)") << endl;
       // cout << (*i)->unparseToString() << endl;
          if (isSgExprStatement(*i) && isSgAssignOp(isSgExprStatement(*i)->get_expression()))
             {
               SgAssignOp* assignment = isSgAssignOp(isSgExprStatement(*i)->get_expression());
            // cout << "In assignment statement " << assignment->unparseToString() << endl;
               if (value)
                    replaceCopiesOfExpression(var, value, assignment->get_rhs_operand());
               if (isSgVarRefExp(assignment->get_lhs_operand()) && isSgVarRefExp(var))
                  {
                    SgVarRefExp* vr = isSgVarRefExp(assignment->get_lhs_operand());
                    if (vr->get_symbol()->get_declaration() == isSgVarRefExp(var)->get_symbol()->get_declaration())
                       {
                         value = assignment->get_rhs_operand();
                         stmts_to_remove.push_back(*i);
                       }
                  }
             }
            else
             {
               if (value)
                    replaceCopiesOfExpression(var, value, *i);
             }
        }

     for (vector<SgStatement*>::iterator i = stmts_to_remove.begin(); i != stmts_to_remove.end(); ++i) {
          stmts.erase(std::find(stmts.begin(), stmts.end(), *i));
     }

     if (value)
        {
       // DQ (12/17/2006): Separate out the construction of the SgAssignOp from the SgExprStatement to support debugging and testing.
       // stmts.push_back(new SgExprStatement(SgNULL_FILE, new SgAssignOp(SgNULL_FILE, var, value)));
          var->set_lvalue(true);
          SgAssignOp* assignmentOperator = new SgAssignOp(SgNULL_FILE, var, value);
          var->set_parent(assignmentOperator);
          value->set_parent(assignmentOperator);

          printf ("In simpleUndoFiniteDifferencingOne(): assignmentOperator = %p \n",assignmentOperator);

       // DQ: Note that the parent of the SgExprStatement will be set in AST post-processing (or it should be).
          SgExprStatement* es = new SgExprStatement(SgNULL_FILE, assignmentOperator);
          assignmentOperator->set_parent(es);
          stmts.push_back(es);
          es->set_parent(body);
        }
   }

class FindForStatementsVisitor: public AstSimpleProcessing {
  vector<SgForStatement*>& stmts;

  public:
  FindForStatementsVisitor(vector<SgForStatement*>& stmts):
    stmts(stmts) {}

  virtual void visit(SgNode* n) {
    if (isSgForStatement(n))
      stmts.push_back(isSgForStatement(n));
  }
};

// Move variables declared in a for statement to just outside that statement.
void moveForDeclaredVariables(SgNode* root)
   {
     vector<SgForStatement*> for_statements;
     FindForStatementsVisitor(for_statements).traverse(root, preorder);

     for (unsigned int i = 0; i < for_statements.size(); ++i)
        {
          SgForStatement* stmt = for_statements[i];
#ifdef FD_DEBUG
          cout << "moveForDeclaredVariables: " << stmt->unparseToString() << endl;
#endif
          SgForInitStatement* init = stmt->get_for_init_stmt();
          if (!init) continue;
          SgStatementPtrList& inits = init->get_init_stmt();
          vector<SgVariableDeclaration*> decls;
          for (SgStatementPtrList::iterator j = inits.begin(); j != inits.end(); ++j) {
            SgStatement* one_init = *j;
            if (isSgVariableDeclaration(one_init))
            {
              decls.push_back(isSgVariableDeclaration(one_init));
            }
          }
          if (decls.empty()) continue;
          SgStatement* parent = isSgStatement(stmt->get_parent());
          assert (parent);
          SgBasicBlock* bb = new SgBasicBlock(SgNULL_FILE);
          stmt->set_parent(bb);
          bb->set_parent(parent);
          SgStatementPtrList ls;
          for (unsigned int j = 0; j < decls.size(); ++j)
             {
               for (SgInitializedNamePtrList::iterator k = decls[j]->get_variables().begin(); k != decls[j]->get_variables().end(); ++k)
                  {
#ifdef FD_DEBUG
                    cout << "Working on variable " << (*k)->get_name().getString() << endl;
#endif
                    SgVariableSymbol* sym = new SgVariableSymbol(*k);
                    bb->insert_symbol((*k)->get_name(), sym);
                    (*k)->set_scope(bb);
                    SgAssignInitializer* kinit = 0;
                    if (isSgAssignInitializer((*k)->get_initializer()))
                       {
                         kinit = isSgAssignInitializer((*k)->get_initializer());
                         (*k)->set_initializer(0);
                       }

                    if (kinit)
                       {
                         SgVarRefExp* vr = new SgVarRefExp(SgNULL_FILE, sym);
                         vr->set_endOfConstruct(SgNULL_FILE);
                         vr->set_lvalue(true);
                         SgAssignOp* assignment = new SgAssignOp(SgNULL_FILE,vr,kinit->get_operand());
                         vr->set_parent(assignment);
                         kinit->get_operand()->set_parent(assignment);
                         SgExprStatement* expr = new SgExprStatement(SgNULL_FILE, assignment);
                         assignment->set_parent(expr);
                         ls.push_back(expr);
                         expr->set_parent(init);
                       }
                  }

#if 0
               SgStatementPtrList::iterator fiiter = std::find(inits.begin(), inits.end(), decls[j]);
               assert (fiiter != inits.end());
               size_t idx = fiiter - inits.begin();
               inits.erase(inits.begin() + idx);
               inits.insert(inits.begin() + idx, ls.begin(), ls.end());
#endif
               bb->get_statements().push_back(decls[j]);
               decls[j]->set_parent(bb);
             }
          inits = ls;
          bb->get_statements().push_back(stmt);
       // printf ("In moveForDeclaredVariables(): parent = %p = %s bb = %p stmt = %p = %s \n",parent,parent->class_name().c_str(),bb,stmt,stmt->class_name().c_str());
          ROSE_ASSERT(stmt->get_parent() == bb);
          parent->replace_statement(stmt, bb);
        }
   }


class FdFindFunctionsVisitor: public AstSimpleProcessing {
  public:
  vector<SgFunctionDefinition*> functions;

  virtual void visit(SgNode* n) {
    if (isSgFunctionDefinition(n)) {
      functions.push_back(isSgFunctionDefinition(n));
    }
  }
};

class FdFindInitnamesVisitor: public AstSimpleProcessing {
  public:
  vector<SgInitializedName*> initnames;

  virtual void visit(SgNode* n) {
#ifdef FD_DEBUG
    cout << "FdFindInitnamesVisitor " << n->class_name() << " " << n << endl;
#endif
    if (isSgInitializedName(n)) {
#ifdef FD_DEBUG
      cout << "Init name " << isSgInitializedName(n)->get_name().getString() << endl;
#endif
      initnames.push_back(isSgInitializedName(n));
    }
#ifdef FD_DEBUG
    if (isSgVarRefExp(n)) {
      cout << "Var ref " << isSgVarRefExp(n)->get_symbol()->get_name().getString() << endl;
    }
#endif
  }
};

class FdFindMultiplicationsVisitor: public AstSimpleProcessing {
  public:
  vector<SgMultiplyOp*> exprs;

  virtual void visit(SgNode* n) {
    if (isSgMultiplyOp(n)) {
      exprs.push_back(isSgMultiplyOp(n));
    }
  }
};

class IsModifiedBadlyVisitor: public AstSimpleProcessing {
  SgInitializedName* initname;
  bool& safe;

  public:
  IsModifiedBadlyVisitor(SgInitializedName* initname, bool& safe):
    initname(initname), safe(safe) {}

  virtual void visit(SgNode* n) {
    SgVarRefExp* vr = isSgVarRefExp(n);
    if (vr && vr->get_symbol()->get_declaration() == initname) {
      switch (vr->get_parent()->variantT()) {
        case V_SgReturnStmt:
        case V_SgExprStatement:
        case V_SgIfStmt:
        case V_SgWhileStmt:
        case V_SgDoWhileStmt:
        case V_SgSwitchStatement:
        case V_SgCaseOptionStmt:
        case V_SgForStatement:
        case V_SgForInitStatement:
#ifdef FD_DEBUG
        cout << "Statement: Variable " << initname->get_name().getString() << " is safe" << endl;
#endif
        // Safe
        break;

        case V_SgPlusPlusOp:
        case V_SgMinusMinusOp:
#ifdef FD_DEBUG
        cout << "Inc/dec: Variable " << initname->get_name().getString() << " is safe" << endl;
#endif
        // Safe
        break;

        case V_SgAddOp:
        case V_SgSubtractOp:
        case V_SgMinusOp:
        case V_SgUnaryAddOp:
        case V_SgNotOp:
        case V_SgPointerDerefExp:
        case V_SgBitComplementOp:
        case V_SgThrowOp:
        case V_SgEqualityOp:
        case V_SgLessThanOp:
        case V_SgLessOrEqualOp:
        case V_SgGreaterThanOp:
        case V_SgGreaterOrEqualOp:
        case V_SgNotEqualOp:
        case V_SgMultiplyOp:
        case V_SgDivideOp:
        case V_SgIntegerDivideOp:
        case V_SgModOp:
        case V_SgAndOp:
        case V_SgOrOp:
        case V_SgBitAndOp:
        case V_SgBitOrOp:
        case V_SgBitXorOp:
        case V_SgCommaOpExp:
        case V_SgLshiftOp:
        case V_SgRshiftOp:
        case V_SgAssignInitializer:
#ifdef FD_DEBUG
        cout << "Non-mutating: Variable " << initname->get_name().getString() << " is safe" << endl;
#endif
        // Safe
        break;

        case V_SgExprListExp:
        if (isSgFunctionCallExp(n->get_parent()->get_parent())) {
          if (isPotentiallyModified(vr, n->get_parent()->get_parent())) {
#ifdef FD_DEBUG
            cout << "Function call: Variable " << initname->get_name().getString() << " is unsafe" << endl;
#endif
            safe = false;
          }
        } else if (isSgConstructorInitializer(n->get_parent()->get_parent())) {
#ifdef FD_DEBUG
          cout << "Constructor: Variable " << initname->get_name().getString() << " is unsafe" << endl;
#endif
          safe = false;
          // FIXME: constructors
        } else {
          cerr << n->get_parent()->get_parent()->sage_class_name() << endl;
          assert (!"Unknown SgExprListExp case");
        }
        break;

        case V_SgAssignOp:
        case V_SgPlusAssignOp:
        case V_SgMinusAssignOp: {
          SgBinaryOp* binop = isSgBinaryOp(vr->get_parent());
          SgExpression* rhs = binop->get_rhs_operand();
          bool lhs_good = (binop->get_lhs_operand() == vr);
#ifdef FD_DEBUG
          cout << "Assign case for " << initname->get_name().getString() << endl;
          cout << "lhs_good = " << (lhs_good ? "true" : "false") << endl;
#endif
          SgAddOp* rhs_a = isSgAddOp(rhs);
          SgExpression* rhs_a_lhs = rhs_a ? rhs_a->get_lhs_operand() : 0;
          SgExpression* rhs_a_rhs = rhs_a ? rhs_a->get_rhs_operand() : 0;
          if (lhs_good) {
            if (isSgValueExp(binop->get_rhs_operand())) {
              // Safe
            } else if (isSgVarRefExp(rhs)) {
              // Safe
            } else if (isSgCastExp(binop->get_rhs_operand()) &&
                       isSgValueExp(isSgCastExp(binop->get_rhs_operand())
                        ->get_operand())) {
              // Safe
            } else if (isSgAssignOp(binop) && 
                       rhs_a &&
                       ((isSgVarRefExp(rhs_a_lhs) && 
                          isSgVarRefExp(rhs_a_lhs)->get_symbol()
                             ->get_declaration() == initname) ||
                        (isSgVarRefExp(rhs_a_rhs) && 
                          isSgVarRefExp(rhs_a_rhs)->get_symbol()
                             ->get_declaration() == initname))) {
              // Safe
            } else {
#ifdef FD_DEBUG
              cout << "Assign: Variable " << initname->get_name().str() << " is unsafe because of " << binop->unparseToString() << ": " << binop->get_rhs_operand()->sage_class_name() << endl;
#endif
              safe = false;
            }
          } else {
            // Safe: RHS of assignment
          }
        }
        break;

        default: {
#ifdef FD_DEBUG
          cout << "Default: Variable " << initname->get_name().str() << " is unsafe because of " << vr->get_parent()->unparseToString() << ": " << vr->get_parent()->sage_class_name() << endl;
#endif
          safe = false;
        }
        break;
      }
    }
  }
};

// Do a simple form of finite differencing on all functions contained within
// root (which should be a project, file, or function definition).
void simpleIndexFiniteDifferencing(SgNode* root) {
  moveForDeclaredVariables(root);
  SgNode* proj = root;
  while (proj && !isSgProject(proj)) proj = proj->get_parent();
  ROSE_ASSERT (proj);
  AstTests::runAllTests(isSgProject(proj));
  rewrite(getAlgebraicRules(), root); // This might modify root
  FdFindFunctionsVisitor ffv;
  ffv.traverse(root, preorder);
  for (unsigned int x = 0; x < ffv.functions.size(); ++x) {
#ifdef FD_DEBUG
    cout << "Working on function " << ffv.functions[x]->get_declaration()->get_name().str() << endl;
#endif
    SgBasicBlock* body = ffv.functions[x]->get_body();
    FdFindInitnamesVisitor fiv;
    fiv.traverse(ffv.functions[x]->get_declaration(), preorder);
    vector<SgInitializedName*> initnames = fiv.initnames;
    set<SgInitializedName*> safe_vars;
    for (vector<SgInitializedName*>::iterator i = initnames.begin();
         i != initnames.end(); ++i) {
      SgInitializedName* initname = *i;
#ifdef FD_DEBUG
      cout << "Found variable " << initname->get_name().str() << endl;
#endif
      bool safe = true;
      IsModifiedBadlyVisitor(initname, safe).traverse(body, preorder);
      if (safe)
        safe_vars.insert(initname);
#ifdef FD_DEBUG
      cout << "Variable " << initname->get_name().str() << " is " << (safe ? "" : "not ") << "safe" << endl;
#endif
    }
    FdFindMultiplicationsVisitor fmv;
    fmv.traverse(body, postorder);
    vector<SgMultiplyOp*> mult_exprs;
    for (unsigned int i = 0; i < fmv.exprs.size(); ++i) {
      bool alreadyProcessed = false;
      for (unsigned int j = 0; j < i; ++j) {
        if (expressionTreeEqual(fmv.exprs[i], fmv.exprs[j])) {
          alreadyProcessed = true;
          break;
        }
      }
      if (alreadyProcessed)
        continue;

#ifdef FD_DEBUG
      cout << "Testing expression " << fmv.exprs[i]->unparseToString() << " for possible FD" << endl;
#endif

      SgExpression* expr1 = fmv.exprs[i]->get_lhs_operand();
      SgExpression* expr2 = fmv.exprs[i]->get_rhs_operand();
      bool isConst1 = isSgValueExp(expr1);
      bool isSafeVar1 = isSgVarRefExp(expr1) && 
                        safe_vars.find(isSgVarRefExp(expr1)->get_symbol()
                                        ->get_declaration())
                          != safe_vars.end();
      bool isGood1 = isConst1 || isSafeVar1;
      bool isConst2 = isSgValueExp(expr2);
      bool isSafeVar2 = isSgVarRefExp(expr2) && 
                        safe_vars.find(isSgVarRefExp(expr2)->get_symbol()
                                        ->get_declaration())
                          != safe_vars.end();
      bool isGood2 = isConst2 || isSafeVar2;
#ifdef FD_DEBUG
      cout << boolalpha << "isGood1 = " << isGood1 << ", isGood2 = " << isGood2 << endl;
#endif
      if (isGood1 && isGood2) {
#ifdef FD_DEBUG
        cout << "Expression is good to run FD on" << endl;
#endif
        mult_exprs.push_back(fmv.exprs[i]);
      } else {
#ifdef FD_DEBUG
        cout << "Expression is not good to run FD on" << endl;
#endif
      }
    }
    for (int i = mult_exprs.size() - 1; i >= 0; --i)
      doFiniteDifferencingOne(mult_exprs[i], body, 
                              getFiniteDifferencingRules());

    SgNode* bodyCopyForRewrite = body;
    rewrite(getAlgebraicRules(), bodyCopyForRewrite); // This might update bodyCopyForRewrite
    ROSE_ASSERT (isSgBasicBlock(bodyCopyForRewrite));
    body = isSgBasicBlock(bodyCopyForRewrite);
  }
}

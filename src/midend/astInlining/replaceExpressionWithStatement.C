
#include "rose.h"
#include "rewrite.h"
#include <iostream>
#include <iomanip>
#include <vector>

// DQ (8/1/2005): test use of new static function to create 
// Sg_File_Info object that are marked as transformations
#undef SgNULL_FILE
#define SgNULL_FILE Sg_File_Info::generateDefaultFileInfoForTransformationNode()

#include "replaceExpressionWithStatement.h"
#include "typeTraits.h"
#include "inlinerSupport.h"

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

// void FixSgTree(SgNode*);

int gensym_counter = 0;

// Replace an assignment statement with the result of a statement
// generator.
// Assumptions: from has assignment expr at top level of statement
//              from is not in the test of a loop or if statement
//              not currently traversing from
void replaceAssignmentStmtWithStatement(SgExprStatement* from,
                                        StatementGenerator* to) {
  assert(from);
  assert(to);
// SgAssignOp* assignment = isSgAssignOp(from->get_expression_root()->get_operand_i());
  SgAssignOp* assignment = isSgAssignOp(from->get_expression());
  assert (assignment);
  SgVarRefExp* var = isSgVarRefExp(assignment->get_lhs_operand());
  assert (var);
  SgStatement* replacement = to->generate(var);
  // replacement->set_parent(from->get_parent());
  myStatementInsert(from, replacement, false);
  myRemoveStatement(from);
}

// Get the expression root of an expression.  Needs to be moved.
SgExpression* getRootOfExpression(SgExpression* n)
   {
     ROSE_ASSERT(n);
#if 0
     std::cout << "Starting getRootOfExpression on 0x" << std::hex << (int)n << ", which has type " << n->sage_class_name() << std::endl;
#endif

     SgNode* parent = n->get_parent();
     while (isSgExpression(parent) != NULL)
        {
       // cout << "n is " << n->sage_class_name() << ": " << n->unparseToString() << endl;
          n = isSgExpression(parent);
          parent = n->get_parent();
#if 0
          std::cout << "Continuing getRootOfExpression on 0x" << std::hex << (int)n;
          if (n) std::cout << ", which has type " << n->sage_class_name();
          std::cout << std::endl;
#endif
          ROSE_ASSERT(n != NULL);
       }

     return n;
   }

// Get the statement of an expression.  Somewhat smarter than the standard
// version of this routine, but needs to be merged.
SgStatement* getStatementOfExpression(SgNode* n)
   {
     assert(n);
  // std::cout << "Starting getStatementOfExpression on 0x" << std::hex << (int)n << ", which has type " << n->sage_class_name() << std::endl;

     SgExpression* expression = isSgExpression(n);
     ROSE_ASSERT(expression != NULL);
     SgExpression* rootExpression = getRootOfExpression(expression);
     ROSE_ASSERT(rootExpression->get_parent() != NULL);
     SgInitializedName* initializedName = isSgInitializedName(rootExpression->get_parent());

     SgNode* root = rootExpression;
     if (initializedName != NULL)
        root = initializedName;

     ROSE_ASSERT(root != NULL);
     SgStatement* statement = isSgStatement(root->get_parent());
     ROSE_ASSERT(statement != NULL);
     return statement;
   }

// Get the initializer containing an expression if it is within an
// initializer.
SgInitializer* getInitializerOfExpression(SgExpression* n) {
  assert(n);
#if 0
  std::cout << "Starting getInitializerOfExpression on 0x" << std::hex << (int)n << ", which has type " << n->sage_class_name() << std::endl;
#endif
  while (!isSgInitializer(n)) {
    n = isSgExpression(n->get_parent());
#if 0
    std::cout << "Continuing getInitializerOfExpression on 0x" << std::hex << (int)n;
    if (n) std::cout << ", which has type " << n->sage_class_name();
    std::cout << std::endl;
#endif
    assert(n);
  }
  return isSgInitializer(n);
}

// Insert a new statement before or after a target statement.  If
// allowForInit is true, the new statement can be inserted into the
// initializer of a for statement.
// Needs to be merged
void myStatementInsert ( SgStatement* target, SgStatement* newstmt, bool before, bool allowForInit )
   {
     ROSE_ASSERT(target != NULL);
     ROSE_ASSERT(newstmt != NULL);
#if 0
     printf ("In inlining: myStatementInsert(): newstmt = %p = %s \n",newstmt,newstmt->class_name().c_str());
     printf ("In inlining: myStatementInsert(): target = %p = %s \n",target,target->class_name().c_str());
#endif
     SgStatement* parent = isSgStatement(target->get_parent());
#if 0
     if (parent == NULL)
        {
          ROSE_ASSERT(target->get_file_info() != NULL);
          target->get_file_info()->display("problem IR node: debug");

          if (target != NULL)
             {
            // printf ("In inlining: myStatementInsert(): target->get_parent() = %p = %s \n",target->get_parent(),target->get_parent()->class_name().c_str());
               printf ("In inlining: myStatementInsert(): target->get_parent() = %p \n",target->get_parent());
             }
        }
#endif
  // cerr << "1: target is a " << target->sage_class_name() << ", target->get_parent() is a " << target->get_parent()->sage_class_name() << endl;
     if (isSgIfStmt(parent) && isSgIfStmt(parent)->get_conditional() == target)
        {
          target = parent;
          parent = isSgScopeStatement(target->get_parent());
        }

  // printf ("allowForInit = %s \n",allowForInit ? "true" : "false");
     if (isSgForInitStatement(target->get_parent()) && !allowForInit)
        {
          target = isSgScopeStatement(target->get_parent()->get_parent());
          parent = isSgScopeStatement(target->get_parent());
          assert (target);
        }

     ROSE_ASSERT(target != NULL);
#if 0
  // DQ (8/1/2005): This fails because the parent at some point is not set and the unparseToString detects this (likely in qualifier generation)
     cerr << "2: target is a " << target->sage_class_name() << ", target->get_parent() is a " << target->get_parent()->sage_class_name() << endl;
     ROSE_ASSERT(parent != NULL);
     if (parent->get_parent() == NULL)
        {
          printf ("Found null parent of %p = %s \n",parent,parent->class_name().c_str());
        }
     ROSE_ASSERT(parent->get_parent() != NULL);
     cerr << "2: parent is a " << parent->sage_class_name() << ", parent->get_parent() is a " << parent->get_parent()->sage_class_name() << endl;
  // cerr << "2: target is " << target->unparseToString() << ", target->get_parent() is " << target->get_parent()->unparseToString() << endl;
#endif
     ROSE_ASSERT (parent);
     SgStatementPtrList* siblings_ptr;
     if (isSgForInitStatement(target->get_parent()))
        {
          siblings_ptr = &isSgForInitStatement(target->get_parent())->get_init_stmt();
        }
       else
        {
          assert (parent);
          if (isSgScopeStatement(parent))
             {
               ROSE_ASSERT(parent != NULL);
               siblings_ptr = &isSgScopeStatement(parent)->getStatementList();
             }
            else
             {
               assert (!"Bad parent type");
             }
        }

     ROSE_ASSERT(siblings_ptr != NULL);
     ROSE_ASSERT(target != NULL);

     SgStatementPtrList& siblings = *siblings_ptr;
     SgStatementPtrList::iterator stmt_iter = std::find(siblings.begin(), siblings.end(), target);
     ROSE_ASSERT (stmt_iter != siblings.end());

     if (!before)
          ++stmt_iter;

     newstmt->set_parent(parent);
     siblings.insert(stmt_iter, newstmt);
   }

class AndOpGenerator: public StatementGenerator
   {
     SgAndOp* op;

     public:
          AndOpGenerator(SgAndOp* op): op(op) {}

          virtual SgStatement* generate(SgExpression* lhs)
             {
               SgTreeCopy treeCopy;
               SgExpression* lhsCopy = isSgExpression(lhs->copy(treeCopy));
               ROSE_ASSERT (lhsCopy);
               SgExpression* assignOpTrue = new SgAssignOp(SgNULL_FILE, lhs, op->get_rhs_operand());
	       assignOpTrue->set_endOfConstruct(SgNULL_FILE);
               lhs->set_parent(assignOpTrue);
               op->get_rhs_operand()->set_parent(assignOpTrue);
               SgStatement* statementTrue = new SgExprStatement(SgNULL_FILE, assignOpTrue);
	       statementTrue->set_endOfConstruct(SgNULL_FILE);
               assignOpTrue->set_parent(statementTrue);
               SgBasicBlock* bbTrue = new SgBasicBlock(SgNULL_FILE, statementTrue);
	       bbTrue->set_endOfConstruct(SgNULL_FILE);
               statementTrue->set_parent(bbTrue);
               SgExpression* falseValue = new SgBoolValExp(SgNULL_FILE, false);
               SgExpression* assignOpFalse = new SgAssignOp(SgNULL_FILE, lhsCopy, falseValue);
	       assignOpFalse->set_endOfConstruct(SgNULL_FILE);
               lhsCopy->set_parent(assignOpFalse);
               falseValue->set_parent(assignOpFalse);
               SgStatement* statementFalse = new SgExprStatement(SgNULL_FILE, assignOpFalse);
	       statementFalse->set_endOfConstruct(SgNULL_FILE);
               assignOpFalse->set_parent(statementFalse);
               SgBasicBlock* bbFalse = new SgBasicBlock(SgNULL_FILE, statementFalse);
	       bbFalse->set_endOfConstruct(SgNULL_FILE);
               statementFalse->set_parent(bbFalse);
               SgExprStatement* ifTest = new SgExprStatement(SgNULL_FILE, op->get_lhs_operand());
	       ifTest->set_endOfConstruct(SgNULL_FILE);
               op->get_lhs_operand()->set_parent(ifTest);
               SgIfStmt* tree =
                    new SgIfStmt(SgNULL_FILE, ifTest, bbTrue, bbFalse);
               ifTest->set_parent(tree);
               bbTrue->set_parent(tree);
               bbFalse->set_parent(tree);
               return tree;
             }
   };

class OrOpGenerator: public StatementGenerator
   {
     SgOrOp* op;

     public:
          OrOpGenerator(SgOrOp* op): op(op) {}

          virtual SgStatement* generate(SgExpression* lhs)
             {
               SgTreeCopy treeCopy;
               SgExpression* lhsCopy = isSgExpression(lhs->copy(treeCopy));
               ROSE_ASSERT (lhsCopy);
               SgExpression* trueValue = new SgBoolValExp(SgNULL_FILE, true);
               SgExpression* assignOpTrue = new SgAssignOp(SgNULL_FILE, lhs, trueValue);
	       assignOpTrue->set_endOfConstruct(SgNULL_FILE);
               lhs->set_parent(assignOpTrue);
               trueValue->set_parent(assignOpTrue);
               SgStatement* statementTrue = new SgExprStatement(SgNULL_FILE, assignOpTrue);
	       statementTrue->set_endOfConstruct(SgNULL_FILE);
               assignOpTrue->set_parent(statementTrue);
               SgBasicBlock* bbTrue = new SgBasicBlock(SgNULL_FILE, statementTrue);
	       bbTrue->set_endOfConstruct(SgNULL_FILE);
               statementTrue->set_parent(bbTrue);
               SgExpression* assignOpFalse = new SgAssignOp(SgNULL_FILE, lhsCopy, op->get_rhs_operand());
	       assignOpFalse->set_endOfConstruct(SgNULL_FILE);
               lhsCopy->set_parent(assignOpFalse);
               op->get_rhs_operand()->set_parent(assignOpFalse);
               SgStatement* statementFalse = new SgExprStatement(SgNULL_FILE, assignOpFalse);
	       statementFalse->set_endOfConstruct(SgNULL_FILE);
               assignOpFalse->set_parent(statementFalse);
               SgBasicBlock* bbFalse = new SgBasicBlock(SgNULL_FILE, statementFalse);
	       bbFalse->set_endOfConstruct(SgNULL_FILE);
               statementFalse->set_parent(bbFalse);
               SgExprStatement* ifTest = new SgExprStatement(SgNULL_FILE, op->get_lhs_operand());
	       ifTest->set_endOfConstruct(SgNULL_FILE);
               op->get_lhs_operand()->set_parent(ifTest);
               SgIfStmt* tree =
                    new SgIfStmt(SgNULL_FILE, ifTest, bbTrue, bbFalse);
               ifTest->set_parent(tree);
               bbTrue->set_parent(tree);
               bbFalse->set_parent(tree);
               return tree;
             }
   };

class ConditionalExpGenerator: public StatementGenerator
   {
     SgConditionalExp* op;

     public:
          ConditionalExpGenerator(SgConditionalExp* op): op(op) {}

          virtual SgStatement* generate(SgExpression* lhs)
             {
               SgTreeCopy treeCopy;
               SgExpression* lhsCopy = isSgExpression(lhs->copy(treeCopy));
               ROSE_ASSERT (lhsCopy);
               SgExpression* assignOpTrue = new SgAssignOp(SgNULL_FILE, lhs, op->get_true_exp());
	       assignOpTrue->set_endOfConstruct(SgNULL_FILE);
               lhs->set_parent(assignOpTrue);
               op->get_true_exp()->set_parent(assignOpTrue);
               SgStatement* statementTrue = new SgExprStatement(SgNULL_FILE, assignOpTrue);
	       statementTrue->set_endOfConstruct(SgNULL_FILE);
               assignOpTrue->set_parent(statementTrue);
               SgBasicBlock* bbTrue = new SgBasicBlock(SgNULL_FILE, statementTrue);
	       bbTrue->set_endOfConstruct(SgNULL_FILE);
               statementTrue->set_parent(bbTrue);
               SgExpression* assignOpFalse = new SgAssignOp(SgNULL_FILE, lhsCopy, op->get_false_exp());
	       assignOpFalse->set_endOfConstruct(SgNULL_FILE);
               lhsCopy->set_parent(assignOpFalse);
               op->get_false_exp()->set_parent(assignOpFalse);
               SgStatement* statementFalse = new SgExprStatement(SgNULL_FILE, assignOpFalse);
	       statementFalse->set_endOfConstruct(SgNULL_FILE);
               assignOpFalse->set_parent(statementFalse);
               SgBasicBlock* bbFalse = new SgBasicBlock(SgNULL_FILE, statementFalse);
	       bbFalse->set_endOfConstruct(SgNULL_FILE);
               statementFalse->set_parent(bbFalse);
               SgExprStatement* ifTest = new SgExprStatement(SgNULL_FILE, op->get_conditional_exp());
	       ifTest->set_endOfConstruct(SgNULL_FILE);
               op->get_conditional_exp()->set_parent(ifTest);
               SgIfStmt* tree =
                    new SgIfStmt(SgNULL_FILE, ifTest, bbTrue, bbFalse);
               ifTest->set_parent(tree);
               bbTrue->set_parent(tree);
               bbFalse->set_parent(tree);
               return tree;
             }
   };

// Replace the expression "from" with another expression "to", wherever it
// appears in the AST.  The expression "from" is not deleted, and may be
// reused elsewhere in the AST.
void replaceExpressionWithExpression(SgExpression* from, SgExpression* to) {
  SgNode* fromparent = from->get_parent();

  to->set_parent(fromparent);
  if (isSgExprStatement(fromparent)) {
    isSgExprStatement(fromparent)->set_expression(to);
  } else if (isSgReturnStmt(fromparent)) {
    isSgReturnStmt(fromparent)->set_expression(to);
  } else if (isSgDoWhileStmt(fromparent)) {
    ROSE_ASSERT (!"FIXME -- this case is present for when the test of a do-while statement is changed to an expression rather than a statement");
  } else if (isSgForStatement(fromparent)) {
    ROSE_ASSERT (isSgForStatement(fromparent)->get_increment() == from);
    isSgForStatement(fromparent)->set_increment(to);
  } else if (isSgExpression(fromparent)) {
    // std::cout << "Unparsed: " << fromparent->sage_class_name() << " --- " << from->unparseToString() << std::endl;
    // std::cout << "Unparsed 2: " << varref->sage_class_name() << " --- " << varref->unparseToString() << std::endl;
    int worked = isSgExpression(fromparent)->replace_expression(from, to);
    ROSE_ASSERT (worked);
  } else {
    ROSE_ASSERT (!"Parent of expression is an unhandled case");
  }
}

// Add a new temporary variable to contain the value of from
// Change reference to from to use this new variable
// Assumptions: from is not within the test of a loop or if
//              not currently traversing from or the statement it is in
SgAssignInitializer* splitExpression(SgExpression* from, string newName) {
  if (!isCopyConstructible(from->get_type())) {
    std::cerr << "Type " << from->get_type()->unparseToString() << " of expression " << from->unparseToString() << " is not copy constructible" << std::endl;
    ROSE_ASSERT (false);
  }
  assert (isCopyConstructible(from->get_type())); // How do we report errors?
  SgStatement* stmt = getStatementOfExpression(from);
  assert (stmt);
  SgScopeStatement* parent = isSgScopeStatement(stmt->get_parent());
  // cout << "parent is a " << (parent ? parent->sage_class_name() : "NULL") << endl;
  if (!parent && isSgForInitStatement(stmt->get_parent()))
    parent = isSgScopeStatement(stmt->get_parent()->get_parent()->get_parent());
  assert (parent);
  // cout << "parent is a " << parent->sage_class_name() << endl;
  // cout << "parent is " << parent->unparseToString() << endl;
  // cout << "stmt is " << stmt->unparseToString() << endl;
  SgName varname = "rose_temp__";
  if (newName == "") {
    varname << ++gensym_counter;
  } else {
    varname = newName;
  }
  SgType* vartype = from->get_type();
  SgNode* fromparent = from->get_parent();
  vector<SgExpression*> ancestors;
  for (SgExpression *expr = from, *anc = isSgExpression(fromparent); anc != 0;
       expr = anc, anc = isSgExpression(anc->get_parent())) {
    if ((isSgAndOp(anc) && expr != isSgAndOp(anc)->get_lhs_operand()) ||
        (isSgOrOp(anc) && expr != isSgOrOp(anc)->get_lhs_operand()) ||
        (isSgConditionalExp(anc) && expr != isSgConditionalExp(anc)->get_conditional_exp()))
      ancestors.push_back(anc); // Closest first
  }
  // cout << "This expression to split has " << ancestors.size() << " ancestor(s)" << endl;
  for (vector<SgExpression*>::reverse_iterator ai = ancestors.rbegin(); ai != ancestors.rend(); ++ai)
  {
    StatementGenerator* gen;
    switch ((*ai)->variantT()) {
      case V_SgAndOp: 
         gen = new AndOpGenerator(isSgAndOp(*ai)); break;
      case V_SgOrOp:
         gen = new OrOpGenerator(isSgOrOp(*ai)); break;
      case V_SgConditionalExp:
         gen = new ConditionalExpGenerator(isSgConditionalExp(*ai)); break;
      default: assert (!"Should not happen");
    }
    replaceExpressionWithStatement(*ai, gen);
    delete gen;
  }
  if (ancestors.size() != 0) {
    return splitExpression(from); 
    // Need to recompute everything if there were ancestors
  }
  SgVariableDeclaration* vardecl =
    new SgVariableDeclaration(SgNULL_FILE, varname, vartype, 0);
  vardecl->set_endOfConstruct(SgNULL_FILE);
  vardecl->get_definition()->set_endOfConstruct(SgNULL_FILE);
  vardecl->set_definingDeclaration(vardecl);
  SgInitializedName* initname = 
    lastElementOfContainer(vardecl->get_variables());
  // initname->set_endOfConstruct(SgNULL_FILE);
  SgVariableSymbol* sym = new SgVariableSymbol(initname);
  assert (sym);
  SgVarRefExp* varref = new SgVarRefExp(SgNULL_FILE, sym);
  varref->set_endOfConstruct(SgNULL_FILE);
  replaceExpressionWithExpression(from, varref);
  // std::cout << "Unparsed 3: " << fromparent->sage_class_name() << " --- " << fromparent->unparseToString() << endl;
  // cout << "From is a " << from->sage_class_name() << endl;
     SgAssignInitializer* ai = new SgAssignInitializer(SgNULL_FILE, from, vartype);
     ai->set_endOfConstruct(SgNULL_FILE);
     from->set_parent(ai);
     initname->set_initializer(ai);
     ai->set_parent(initname);
//  }
  initname->set_parent(vardecl);
  initname->set_scope(parent);
  myStatementInsert(stmt, vardecl, true);
  // vardecl->set_parent(stmt->get_parent());
  // FixSgTree(vardecl);
  // FixSgTree(parent);
  parent->insert_symbol(varname, sym);
  sym->set_parent(parent->get_symbol_table());
  return ai;
}

// Convert something like "int a = foo();" into "int a; a = foo();"
SgAssignOp* convertInitializerIntoAssignment(SgAssignInitializer* init)
   {
     assert (isDefaultConstructible(init->get_operand_i()->get_type()));
     SgStatement* stmt = getStatementOfExpression(init);
     assert (stmt);
     SgScopeStatement* parent = isSgScopeStatement(stmt->get_parent());
     if (!parent && isSgForInitStatement(stmt->get_parent()))
          parent = isSgScopeStatement(stmt->get_parent()->get_parent()->get_parent());
     assert (parent);
     SgNode* initparent = init->get_parent();
     assert (initparent);

     SgInitializedName* initname = NULL;
     if (isSgInitializedName(initparent))
          initname = isSgInitializedName(initparent);
       else
          if (isSgVariableDefinition(initparent))
               initname = isSgVariableDefinition(initparent)->get_vardefn();
            else
               if (isSgVariableDeclaration(initparent))
                  {
                    SgInitializedNamePtrList& vars = isSgVariableDeclaration(initparent)->get_variables();
                    for (SgInitializedNamePtrList::iterator i = vars.begin(); i != vars.end(); ++i)
                       {
                         if ((*i)->get_initializer() == init)
                            {
                              initname = *i;
                              break;
                            }
                       }
                  }
                 else
                  {
                    std::cout << "initparent is a " << initparent->sage_class_name() << std::endl;
                    assert (!"Should not happen");
                  }

     assert (initname);
     assert (initname->get_initializer() == init);
     assert (parent);
     SgVariableSymbol* sym = initname->get_scope()->get_symbol_table()->findvar(initname->get_name());
     ROSE_ASSERT (sym);
     SgVarRefExp* vr = new SgVarRefExp(SgNULL_FILE, sym);
     vr->set_endOfConstruct(SgNULL_FILE);
     vr->set_lvalue(true);
     SgAssignOp* assignment = new SgAssignOp(SgNULL_FILE, vr, init->get_operand());
     assignment->set_endOfConstruct(SgNULL_FILE);
     vr->set_parent(assignment);
     init->get_operand()->set_parent(assignment);

     printf ("In convertInitializerIntoAssignment: building SgAssignOp = %p \n",assignment);

     SgExprStatement* assign_stmt = new SgExprStatement(SgNULL_FILE, assignment);
     assign_stmt->set_endOfConstruct(SgNULL_FILE);
     assignment->set_parent(assign_stmt);
     initname->set_initializer(NULL);

  // assignment->set_parent(assign_stmt);
  // cout << "stmt is " << stmt->unparseToString() << endl;
  // cout << "stmt->get_parent() is a " << stmt->get_parent()->sage_class_name() << endl;

     myStatementInsert(stmt, assign_stmt, false);
     assign_stmt->set_parent(parent);
     assign_stmt->get_expression()->set_parent(assign_stmt);

     ROSE_ASSERT(assignment->get_parent() != NULL);

  // FixSgTree(assign_stmt);
  // FixSgTree(parent);

     return assignment;
   }

// Similar to replaceExpressionWithStatement, but with more restrictions.
// Assumptions: from is not within the test of a loop or if
//              not currently traversing from or the statement it is in
void replaceSubexpressionWithStatement(SgExpression* from, StatementGenerator* to)
   {
  // printf ("In replaceSubexpressionWithStatement(from,to): from = %p = %s = %s\n",from,from->class_name().c_str(), from->unparseToString().c_str());

     SgStatement* stmt = getStatementOfExpression(from);

  // printf ("In replaceSubexpressionWithStatement(): from = %p = %s \n",from,from->class_name().c_str());

     if (isSgExprStatement(stmt))
        {
          SgExpression* top = getRootOfExpression(from);

       // printf ("In replaceSubexpressionWithStatement(): top = %p = %s \n",top,top->class_name().c_str());

          if (top == from)
             {
               SgStatement* generated = to->generate(0);
            // printf ("top == from: Setting generated = %p = %s to have parent = %p = %s \n",
            //      generated,generated->class_name().c_str(),stmt->get_parent(),stmt->get_parent()->class_name().c_str());
            // printf ("generated = %s\n", generated->unparseToString().c_str());
               isSgStatement(stmt->get_parent())->replace_statement(stmt, generated);
               generated->set_parent(stmt->get_parent());
               return;
             }
            else
             {
               if (isSgAssignOp(top) && isSgAssignOp(top)->get_rhs_operand() == from)
                  {
                    SgAssignOp* t = isSgAssignOp(top);
                    SgStatement* generated = to->generate(t->get_lhs_operand());
                 // printf ("top != from: Setting generated = %p = %s to have parent = %p = %s \n",
                 //      generated,generated->class_name().c_str(),stmt->get_parent(),stmt->get_parent()->class_name().c_str());
                 // printf ("stmt = %s\n", stmt->unparseToString().c_str());
                 // printf ("stmt->get_parent() = %s\n", stmt->get_parent()->unparseToString().c_str());
                 // printf ("generated = %s\n", generated->unparseToString().c_str());
                    isSgStatement(stmt->get_parent())->replace_statement(stmt, generated);
                    generated->set_parent(stmt->get_parent());
                    return;
                  }
                 else
                  {
                 // printf ("In replaceSubexpressionWithStatement(): Statement not generated \n");
                  }
             }
        }

  // cout << "1: " << getStatementOfExpression(from)->unparseToString() << endl;
     SgAssignInitializer* init = splitExpression(from);
  // cout << "2: " << getStatementOfExpression(from)->unparseToString() << endl;
     convertInitializerIntoAssignment(init);
  // cout << "3: " << getStatementOfExpression(from)->unparseToString() << endl;
  // cout << "3a: " << getStatementOfExpression(from)->get_parent()->unparseToString() << endl;
     SgExprStatement* new_stmt = isSgExprStatement(getStatementOfExpression(from));
     assert (new_stmt != NULL); // Should now have this form because of conversion
     replaceAssignmentStmtWithStatement(new_stmt, to);

  // printf ("In replaceSubexpressionWithStatement: new_stmt = %p = %s \n",new_stmt,new_stmt->class_name().c_str());
  // cout << "4: " << getStatementOfExpression(from)->get_parent()->unparseToString() << endl;
   }

// Change continue statements in a given block of code to gotos to a label
void changeContinuesToGotos(SgStatement* stmt, SgLabelSymbol* label)
   {
     std::vector<SgContinueStmt*> continues = findContinueStmts(stmt);
     for (std::vector<SgContinueStmt*>::iterator i = continues.begin(); i != continues.end(); ++i)
        {
       // LowLevelRewrite::replace(*i, make_unit_list( new SgGotoStatement(SgNULL_FILE, label->get_declaration())));
          SgGotoStatement* gotoStatement = new SgGotoStatement(SgNULL_FILE, label->get_declaration());
	  gotoStatement->set_endOfConstruct(SgNULL_FILE);
       // printf ("Building gotoStatement #1 = %p \n",gotoStatement);
          LowLevelRewrite::replace(*i, make_unit_list( gotoStatement ) );

          ROSE_ASSERT(gotoStatement->get_parent() != NULL);
        }
   }

// Return bool for C++ code, and int for C code
SgType* getBoolType(SgNode* n) {
  bool isC = TransformationSupport::getFile(n)->get_outputLanguage() == SgFile::e_C_output_language;
  if (isC) {
    return SgTypeInt::createType();
  } else {
    return SgTypeBool::createType();
  }
}

// Rewrites a while or for loop so that the official test is changed to
// "true" and what had previously been the test is now an if-break
// combination (with an inverted condition) at the beginning of the loop
// body
//
// For example, "while (a < 5) ++a;" becomes:
// "while (true) {bool temp; temp = (a < 5); if (!temp) break; ++a;}"
template <class LoopStatement>
void pushTestIntoBody(LoopStatement* loopStmt) {
  SgBasicBlock* new_body = new SgBasicBlock(SgNULL_FILE);
  new_body->set_endOfConstruct(SgNULL_FILE);
// printf ("Building IR node #10: new SgBasicBlock = %p \n",new_body);
  SgBasicBlock* old_body = getLoopBody(loopStmt);
  setLoopBody(loopStmt, new_body);
  SgName varname = "rose__temp"; // Does not need to be unique, but must not be used in user code anywhere
  SgVariableDeclaration* new_decl = 
    new SgVariableDeclaration(SgNULL_FILE, varname, getBoolType(loopStmt));
  new_decl->set_endOfConstruct(SgNULL_FILE);
  new_decl->get_definition()->set_endOfConstruct(SgNULL_FILE);
  new_decl->set_definingDeclaration(new_decl);
  SgInitializedName* initname = 
    lastElementOfContainer(new_decl->get_variables());
  // initname->set_endOfConstruct(SgNULL_FILE);
  initname->set_scope(new_body);
  SgVariableSymbol* varsym = new SgVariableSymbol(initname);
  new_body->insert_symbol(varname, varsym);
  varsym->set_parent(new_body->get_symbol_table());
  new_decl->set_parent(new_body);
  new_body->get_statements().push_back(new_decl);

// DQ (11/7/2006): modified this code to account for removal of SgExpressionRoot.
// SgExpressionRoot* root = getLoopConditionRoot(loopStmt);
  SgExpression* root = getLoopConditionRoot(loopStmt);

// DQ (11/7/2006): modified this code to account for removal of SgExpressionRoot.
// DQ (2/21/2006): Make sure that all SgCastExp have there 
// type set since there is no way to figure it out otherwise.
// SgCastExp* cast = new SgCastExp(SgNULL_FILE, root->get_operand_i());
// SgCastExp* cast = new SgCastExp(SgNULL_FILE, root->get_operand_i(),new SgTypeBool());
  SgCastExp* cast = new SgCastExp(SgNULL_FILE, root,getBoolType(loopStmt));
// printf ("pushTestIntoBody(): built SgCastExp = %p (no type specificed) \n",cast);

// DQ (11/7/2006): modified this code to account for removal of SgExpressionRoot.
// root->get_operand_i()->set_parent(cast);
  root->set_parent(cast);
  assert (varsym);
  SgVarRefExp* vr1 = new SgVarRefExp(SgNULL_FILE, varsym);
  vr1->set_endOfConstruct(SgNULL_FILE);
  vr1->set_lvalue(true);
  SgAssignOp* assignment = new SgAssignOp(SgNULL_FILE, vr1, cast);
  assignment->set_endOfConstruct(SgNULL_FILE);
  vr1->set_parent(assignment);
  cast->set_parent(assignment);

  printf ("In pushTestIntoBody: building SgAssignOp = %p \n",assignment);

  SgExprStatement* temp_setup = new SgExprStatement(SgNULL_FILE, assignment);
  temp_setup->set_endOfConstruct(SgNULL_FILE);
  assignment->set_parent(temp_setup);
  new_body->get_statements().push_back(temp_setup);
  temp_setup->set_parent(new_body);
  SgBreakStmt* breakStmt = new SgBreakStmt(SgNULL_FILE);
  SgBasicBlock* breakStmtBlock = new SgBasicBlock(SgNULL_FILE, breakStmt);
  breakStmtBlock->set_endOfConstruct(SgNULL_FILE);
  breakStmt->set_parent(breakStmtBlock);
  SgVarRefExp* varsymVr = new SgVarRefExp(SgNULL_FILE, varsym);
  varsymVr->set_endOfConstruct(SgNULL_FILE);
  SgNotOp* notOp = new SgNotOp(SgNULL_FILE, varsymVr);
  varsymVr->set_parent(notOp);
  SgExprStatement* notOpStmt = new SgExprStatement(SgNULL_FILE, notOp);
  notOpStmt->set_endOfConstruct(SgNULL_FILE);
  notOp->set_parent(notOpStmt);
  SgBasicBlock* emptyElse = new SgBasicBlock(SgNULL_FILE);
  emptyElse->set_endOfConstruct(SgNULL_FILE);
  SgIfStmt* loop_break =
       new SgIfStmt(SgNULL_FILE, notOpStmt, breakStmtBlock, emptyElse);
  notOpStmt->set_parent(loop_break);
  breakStmtBlock->set_parent(loop_break);
  emptyElse->set_parent(loop_break);
#if 0
  printf ("Building IR node #4: loop_break = %p \n",loop_break);
  printf ("Building IR node #11: new SgBasicBlock = %p \n",loop_break->get_true_body());
  printf ("Building IR node #12: new SgBasicBlock = %p \n",loop_break->get_false_body());
#endif
  loop_break->set_parent(new_body);
  new_body->get_statements().push_back(loop_break);
  old_body->set_parent(new_body);
  new_body->get_statements().push_back(old_body);
  // FixSgTree(new_body);
  setLoopConditionRoot(loopStmt, new SgBoolValExp(SgNULL_FILE, true));
}

template <class LoopStatement>
// Add a step statement to the end of a loop body
// Add a new label to the end of the loop, with the step statement after
// it; then change all continue statements in the old loop body into
// jumps to the label
//
// For example:
// while (a < 5) {if (a < -3) continue;} (adding "a++" to end) becomes
// while (a < 5) {if (a < -3) goto label; label: a++;}
void addStepToLoopBody(LoopStatement* loopStmt, SgStatement* step) {
  SgBasicBlock* old_body = getLoopBody(loopStmt);
  SgBasicBlock* new_body = new SgBasicBlock(SgNULL_FILE);
  new_body->set_endOfConstruct(SgNULL_FILE);
// printf ("Building IR node #13: new SgBasicBlock = %p \n",new_body);
  SgName labelname = "rose_label__";
  labelname << ++gensym_counter;
  SgLabelStatement* labelstmt = new SgLabelStatement(SgNULL_FILE, labelname);
  labelstmt->set_endOfConstruct(SgNULL_FILE);
  labelstmt->set_scope(new_body);
  SgLabelSymbol* labelsym = new SgLabelSymbol(labelstmt);
  changeContinuesToGotos(old_body, labelsym);
  new_body->insert_symbol(labelname, labelsym);
  labelsym->set_parent(new_body->get_symbol_table());
  new_body->get_statements().push_back(old_body);
  new_body->get_statements().push_back(labelstmt);

// DQ (6/24/2006): Set the parent
  labelstmt->set_parent(new_body);

  new_body->get_statements().push_back(step);
  step->set_parent(new_body);
  setLoopBody(loopStmt, new_body);
  new_body->set_parent(loopStmt);
}

// Replace a given expression with a list of statements produced by a
// generator.  The generator, when given a variable as input, must produce
// some code which leaves its result in the given variable.  The output
// from the generator is then inserted into the original program in such a
// way that whenever the expression had previously been evaluated, the
// statements produced by the generator are run instead and their result is
// used in place of the expression.
// Assumptions: not currently traversing from or the statement it is in
void
replaceExpressionWithStatement(SgExpression* from, StatementGenerator* to)
   {
  // DQ (3/11/2006): The problem here is that the test expression for a "for loop" (SgForStmt)
  // is assumed to be a SgExpression.  This was changed in Sage III as part of a bugfix and so 
  // the original assumptions upon which this function was based are not incorrect, hence the bug!
  // Note that a number of cases were changed when this fix was made to SageIII (see documentation 
  // for SgScopeStatement).

  // printf ("In replaceExpressionWithStatement(from,to): from = %p = %s \n",from,from->class_name().c_str());
  // printf ("In replaceExpressionWithStatement(from,to): to   = %p = %s \n",to,to->class_name().c_str());

     SgStatement*           enclosingStatement      = getStatementOfExpression(from);
     SgExprStatement*       exprStatement           = isSgExprStatement(enclosingStatement);

     SgForStatement*        forStatement            = isSgForStatement(enclosingStatement);
     SgReturnStmt*          returnStatement         = isSgReturnStmt(enclosingStatement);
     SgVariableDeclaration* varDeclarationStatement = isSgVariableDeclaration(enclosingStatement);

  // These SgScopeStatement constructs have a SgStatement for a condition and thus 
  // represent a special case in the processing in this function.
  // SgIfStmt*          ifStatement     = NULL;
  // SgWhileStmt*       whileStatement  = NULL;
  // SgSwitchStatement* switchStatement = NULL;

  // printf ("replaceExpressionWithStatement: exprStatement = %p \n",exprStatement);

  // DQ (3/11/2006): Bugfix for special cases of conditional that are either SgStatement or SgExpression IR nodes.
#if 0
     printf ("In replaceExpressionWithStatement(from,to): enclosingStatement = %p = %s \n",enclosingStatement,enclosingStatement->class_name().c_str());
     if (exprStatement != NULL)
          printf ("In replaceExpressionWithStatement(from,to): exprStatement = %p = %s \n",exprStatement,exprStatement->class_name().c_str());
     if (forStatement != NULL)
          printf ("In replaceExpressionWithStatement(from,to): forStatement = %p = %s \n",forStatement,forStatement->class_name().c_str());
#endif

     ROSE_ASSERT (exprStatement || forStatement || returnStatement || varDeclarationStatement);

     if (varDeclarationStatement)
        {
          replaceSubexpressionWithStatement(from, to);
        }
       else
        {
       // SgExpressionRoot* root = getRootOfExpression(from);
          SgExpression* root = getRootOfExpression(from);
          ROSE_ASSERT (root);
       // printf ("root = %p \n",root);
             {
               if (forStatement && forStatement->get_increment() == root)
                  {
                 // printf ("Convert step of for statement \n");
                 // Convert step of for statement
                 // for (init; test; e) body; (where e contains from) becomes
                 // for (init; test; ) {
                 //   body (with "continue" changed to "goto label");
                 //   label: e;
                 // }
                 // std::cout << "Converting for step" << std::endl;
		    SgExprStatement* incrStmt = new SgExprStatement(SgNULL_FILE, forStatement->get_increment());
		    incrStmt->set_endOfConstruct(SgNULL_FILE);
                    forStatement->get_increment()->set_parent(incrStmt);
                    addStepToLoopBody(forStatement, incrStmt);
		    SgNullExpression* ne = new SgNullExpression(SgNULL_FILE);
		    ne->set_endOfConstruct(SgNULL_FILE);
                    forStatement->set_increment(ne);
                    replaceSubexpressionWithStatement(from, to);
                  }
                 else
                  {
                    SgStatement* enclosingStmtParent = isSgStatement(enclosingStatement->get_parent());
                    assert (enclosingStmtParent);
                    SgWhileStmt* whileStatement = isSgWhileStmt(enclosingStmtParent);
                    SgDoWhileStmt* doWhileStatement = isSgDoWhileStmt(enclosingStmtParent);
                    SgIfStmt* ifStatement = isSgIfStmt(enclosingStmtParent);
                    SgForStatement* enclosingForStatement = isSgForStatement(enclosingStmtParent);
                  if (enclosingForStatement && enclosingForStatement->get_test() == exprStatement)
                     {
                    // printf ("Found the test in the for loop \n");
                    // ROSE_ASSERT(false);

                    // Convert test of for statement:
                    // for (init; e; step) body; (where e contains from) becomes
                    // for (init; true; step) {
                    //   bool temp;
                    //   temp = e;
                    //   if (!temp) break;
                    //   body;
                    // }
                    // in which "temp = e;" is rewritten further
                    // std::cout << "Converting for test" << std::endl;
                       pushTestIntoBody(enclosingForStatement);
                       replaceSubexpressionWithStatement(from, to);
                     }
                  else if (whileStatement && whileStatement->get_condition() == exprStatement)
                     {
                    // printf ("Convert while statements \n");
                    // Convert while statement:
                    // while (e) body; (where e contains from) becomes
                    // while (true) {
                    //   bool temp;
                    //   temp = e;
                    //   if (!temp) break;
                    //   body;
                    // }
                    // in which "temp = e;" is rewritten further
                    // std::cout << "Converting while test" << std::endl;
                       pushTestIntoBody(whileStatement);
                    // FixSgTree(whileStatement);
                       replaceSubexpressionWithStatement(from, to);
                     } 
                  else if (doWhileStatement && doWhileStatement->get_condition() == exprStatement)
                    {
         // printf ("Convert do-while statements \n");
                   // Convert do-while statement:
                   // do body; while (e); (where e contains from) becomes
                   // {bool temp = true;
                   //  do {
                   //    body (with "continue" changed to "goto label";
                   //    label:
                   //    temp = e;} while (temp);}
                   // in which "temp = e;" is rewritten further
                   // std::cout << "Converting do-while test" << std::endl;
                      SgBasicBlock* new_statement = new SgBasicBlock(SgNULL_FILE);
		      new_statement->set_endOfConstruct(SgNULL_FILE);
                   // printf ("Building IR node #14: new SgBasicBlock = %p \n",new_statement);
                      assert (doWhileStatement->get_parent());
                      new_statement->set_parent(doWhileStatement->get_parent());
                      myStatementInsert(doWhileStatement, new_statement, false);
                      myRemoveStatement(doWhileStatement);
                      SgName varname = "rose__temp"; // Does not need to be unique, but must not be used in user code anywhere
                      SgAssignInitializer* assignInitializer = new SgAssignInitializer(SgNULL_FILE, new SgBoolValExp(SgNULL_FILE, true),getBoolType(doWhileStatement));
                      assignInitializer->set_endOfConstruct(SgNULL_FILE);

                   // SgVariableDeclaration* new_decl = 
         //       new SgVariableDeclaration(SgNULL_FILE, varname, new SgTypeBool(), 
         //           new SgAssignInitializer(SgNULL_FILE, new SgBoolValExp(SgNULL_FILE, true),new SgTypeBool()));
                      SgVariableDeclaration* new_decl = new SgVariableDeclaration(SgNULL_FILE, varname, getBoolType(doWhileStatement), assignInitializer);
		      new_decl->set_endOfConstruct(SgNULL_FILE);
		      new_decl->get_definition()->set_endOfConstruct(SgNULL_FILE);
                      new_decl->set_definingDeclaration(new_decl);
                      SgInitializedName* initname = lastElementOfContainer(new_decl->get_variables());
		      // initname->set_endOfConstruct(SgNULL_FILE);
                      initname->set_scope(new_statement);

         // DQ (12/14/2006): set the parent of the SgAssignInitializer to the variable (SgInitializedName).
            assignInitializer->set_parent(initname);

                      SgVariableSymbol* varsym = new SgVariableSymbol(initname);
                      new_statement->insert_symbol(varname, varsym);
                      varsym->set_parent(new_statement->get_symbol_table());
                      new_decl->set_parent(new_statement);
                      new_statement->get_statements().push_back(new_decl);
                      new_statement->get_statements().push_back(doWhileStatement);
                      doWhileStatement->set_parent(new_statement);
                      assert (varsym);

                   // SgAssignOp* assignment = new SgAssignOp(SgNULL_FILE, new SgVarRefExp(SgNULL_FILE, varsym), new SgCastExp(SgNULL_FILE, root->get_operand_i(), new SgTypeBool()));
                   // SgCastExp* castExp1 = new SgCastExp(SgNULL_FILE, root->get_operand_i(), new SgTypeBool());
                      SgCastExp* castExp1 = new SgCastExp(SgNULL_FILE, root, getBoolType(doWhileStatement));
		      SgVarRefExp* vr = new SgVarRefExp(SgNULL_FILE, varsym);
		      vr->set_endOfConstruct(SgNULL_FILE);
                      vr->set_lvalue(true);
                      SgAssignOp* assignment = new SgAssignOp(SgNULL_FILE, vr, castExp1 );
		      assignment->set_endOfConstruct(SgNULL_FILE);
		      vr->set_parent(assignment);
		      castExp1->set_parent(assignment);
                   // printf ("replaceExpressionWithStatement(): built #1 SgCastExp = %p (type specified) \n",castExp1);

            printf ("In replaceExpressionWithStatement(): building SgAssignOp = %p \n",assignment);

                      SgExprStatement* temp_setup = new SgExprStatement(SgNULL_FILE, assignment);
		      temp_setup->set_endOfConstruct(SgNULL_FILE);

         // DQ (12/14/2006): Uncommented where we set the parent, is not required!
                      assignment->set_parent(temp_setup);

                      addStepToLoopBody(doWhileStatement, temp_setup);
                      SgVarRefExp* varsymVr = new SgVarRefExp(SgNULL_FILE, varsym);
		      varsymVr->set_endOfConstruct(SgNULL_FILE);
                      SgExprStatement* condStmt = new SgExprStatement(SgNULL_FILE, varsymVr);
		      condStmt->set_endOfConstruct(SgNULL_FILE);
                      varsymVr->set_parent(condStmt);
                      doWhileStatement->set_condition(condStmt);
                      condStmt->set_parent(doWhileStatement);
                      replaceSubexpressionWithStatement(from, to);
                    }
                  else if (ifStatement && ifStatement->get_conditional() == exprStatement)
                    {
                      SgBasicBlock* new_statement = new SgBasicBlock(SgNULL_FILE);
		      new_statement->set_endOfConstruct(SgNULL_FILE);
                   // printf ("Building IR node #15: new SgBasicBlock = %p \n",new_statement);
                      assert (ifStatement->get_parent());
                      new_statement->set_parent(ifStatement->get_parent());
                      myStatementInsert(ifStatement, new_statement, false);
                      myRemoveStatement(ifStatement);
                      SgName varname = "rose__temp"; // Does not need to be unique, but must not be used in user code anywhere
                      SgBoolValExp* trueVal = new SgBoolValExp(SgNULL_FILE, true);
                      trueVal->set_endOfConstruct(SgNULL_FILE);
                      SgAssignInitializer* ai = new SgAssignInitializer(SgNULL_FILE, trueVal);
                      ai->set_endOfConstruct(SgNULL_FILE);
                      trueVal->set_parent(ai);
                      SgVariableDeclaration* new_decl = new SgVariableDeclaration(SgNULL_FILE, varname, getBoolType(ifStatement), ai);
		      new_decl->set_endOfConstruct(SgNULL_FILE);
		      new_decl->get_definition()->set_endOfConstruct(SgNULL_FILE);
                      new_decl->set_definingDeclaration(new_decl);
                      SgInitializedName* initname = lastElementOfContainer(new_decl->get_variables());
		      // initname->set_endOfConstruct(SgNULL_FILE);
                      ai->set_parent(initname);
                      initname->set_scope(new_statement);
                      SgVariableSymbol* varsym = new SgVariableSymbol(initname);
                      new_statement->insert_symbol(varname, varsym);
                      varsym->set_parent(new_statement->get_symbol_table());
                      new_decl->set_parent(new_statement);
                      new_statement->get_statements().push_back(new_decl);
                      ifStatement->set_parent(new_statement);
                      assert (varsym);

                   // SgAssignOp* assignment = new SgAssignOp(SgNULL_FILE, new SgVarRefExp(SgNULL_FILE, varsym), new SgCastExp(SgNULL_FILE, root->get_operand_i(), new SgTypeBool()));
                   // SgCastExp* castExp2 = new SgCastExp(SgNULL_FILE, root->get_operand_i(), new SgTypeBool());
                      SgCastExp* castExp2 = new SgCastExp(SgNULL_FILE, root, getBoolType(ifStatement));
		      SgVarRefExp* vr = new SgVarRefExp(SgNULL_FILE, varsym);
		      vr->set_endOfConstruct(SgNULL_FILE);
                      vr->set_lvalue(true);
                      SgAssignOp* assignment = new SgAssignOp(SgNULL_FILE, vr, castExp2 );
		      assignment->set_endOfConstruct(SgNULL_FILE);
		      vr->set_parent(assignment);
		      castExp2->set_parent(assignment);
                   // printf ("replaceExpressionWithStatement(): built #2 SgCastExp = %p (type specified) \n",castExp2);

            printf ("In replaceExpressionWithStatement(): building SgAssignOp = %p \n",assignment);

                      SgExprStatement* temp_setup = new SgExprStatement(SgNULL_FILE, assignment);
		      temp_setup->set_endOfConstruct(SgNULL_FILE);
                      assignment->set_parent(new_statement);
                      new_statement->get_statements().push_back(temp_setup);
                      new_statement->get_statements().push_back(ifStatement);
                      temp_setup->set_parent(new_statement);
                      ifStatement->set_parent(new_statement);
                      SgVarRefExp* vr2 = new SgVarRefExp(SgNULL_FILE, varsym);
		      vr2->set_endOfConstruct(SgNULL_FILE);
                      SgExprStatement* es = new SgExprStatement(SgNULL_FILE, vr2);
		      es->set_endOfConstruct(SgNULL_FILE);
                      vr2->set_parent(es);
                      ifStatement->set_conditional(es);
                      es->set_parent(ifStatement);
                      replaceSubexpressionWithStatement(from, to);
                    }
                  else
                    {
                      // printf ("Handles expression and return statements \n");
                      // Handles expression and return statements
                      // std::cout << "Converting other statement" << std::endl;
                      replaceSubexpressionWithStatement(from, to);
                    }
                  }
             }
        }

  // printf ("Leaving replaceExpressionWithStatement(from,to) \n");
   }


// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "sageBuilder.h"

// DQ (8/1/2005): test use of new static function to create 
// Sg_File_Info object that are marked as transformations
#undef SgNULL_FILE
#define SgNULL_FILE Sg_File_Info::generateDefaultFileInfoForTransformationNode()

#include "inlinerSupport.h"

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

typedef std::set<SgLabelStatement*> SgLabelStatementPtrSet;

class FlattenBlocksVisitor: public AstSimpleProcessing {
  public:
  virtual void visit(SgNode* node) {
    if (isSgBasicBlock(node)) {
      SgBasicBlock* c = isSgBasicBlock(node);
      SgStatementPtrList newStatements;
      for (SgStatementPtrList::const_iterator i = c->get_statements().begin();
           i != c->get_statements().end(); ++i) {
        if (isSgBasicBlock(*i)) {
          SgBasicBlock* c2 = isSgBasicBlock(*i);
          const SgStatementPtrList& c2Stmts = c2->get_statements();
          // We need to prevent a declaration from immediately following a label, as that is illegal
          if (!newStatements.empty() && isSgLabelStatement(newStatements.back()) && !c2Stmts.empty() && isSgVariableDeclaration(c2Stmts.front())) {
            newStatements.push_back(SageBuilder::buildExprStatement(SageBuilder::buildNullExpression()));
          }
          newStatements.insert(newStatements.end(), isSgBasicBlock(*i)->get_statements().begin(), isSgBasicBlock(*i)->get_statements().end());
        } else {
          if (!newStatements.empty() && isSgLabelStatement(newStatements.back()) && isSgVariableDeclaration(*i)) {
            newStatements.push_back(SageBuilder::buildExprStatement(SageBuilder::buildNullExpression()));
          }
          newStatements.push_back(*i);
        }
      }
      if (!newStatements.empty() && isSgLabelStatement(newStatements.back())) {
        // Prevent block from ending with a label
        newStatements.push_back(SageBuilder::buildExprStatement(SageBuilder::buildNullExpression()));
      }
      for (SgStatementPtrList::const_iterator i = newStatements.begin();
           i != newStatements.end(); ++i) {
        (*i)->set_parent(c);
      }
      c->get_statements() = newStatements;
      c->get_symbol_table()->get_table()->clear();
      SageInterface::rebuildSymbolTable(c);
    }
  }
};

// Flatten nested blocks of code in a program.  This code requires that
// variable names in the region being processed are already distinct, and
// it may extend variable lifetimes to the end of the outer block of code.
void flattenBlocks(SgNode* n) {
  FlattenBlocksVisitor().traverse(n, postorder);
}

// Internal routine to check whether a given variable declaration is a
// member variable.
bool isMemberVariable(const SgNode& in) {
  if (!&in) return false;
  SgNode* p = in.get_parent();
  if (isSgClassDefinition(p)) return true;
  if (isSgGlobal(p)) return false;
  return isMemberVariable(*p);
}

// Visitor to rename non-member variables.  Used internally.
class RenameVariablesVisitor: public AstSimpleProcessing
   {
     int counter;

     public:
          RenameVariablesVisitor(): counter(0) {}

          virtual void visit(SgNode* n)
             {
               if (isSgFunctionDefinition(n)) {
                 renameLabels(isSgFunctionDefinition(n), isSgFunctionDefinition(n));
               } else if (isSgInitializedName(n))
                  {
                    SgInitializedName* n2 = isSgInitializedName(n);

                    ROSE_ASSERT(n2->get_file_info() != NULL);

                    if (isMemberVariable(*n2)) return;

                 // JW (7/16/2004): Added patch
                    if (isSgVariableDeclaration(n2->get_parent()))
                       {
                         SgVariableDeclaration* decl = isSgVariableDeclaration(n2->get_parent());
                         if (isSgGlobal(decl->get_parent())) return;
                         if (isSgNamespaceDefinitionStatement(decl->get_parent())) return;
                       }

                    if (isSgCtorInitializerList(n2->get_parent())) return;
                    if (n2->get_name().getString() == "") return;
                    SgName name(n2->get_name());
                    SgSymbolTable* symtab = n2->get_scope()->get_symbol_table();
                    SgSymbol* sym = symtab->find(n2);
                    if (sym) {
                      symtab->remove(sym);
                    }
                    name << "__" << counter++;

                    n2->set_name(name);
                    SgVariableSymbol* n2symbol = new SgVariableSymbol(n2);
                    n2symbol->set_parent(symtab);
                    symtab->insert(name, n2symbol);

                 // printf ("RenameVariablesVisitor(): name = %s scope = %p = %s \n",name.str(),savedScope,savedScope->class_name().c_str());
                    ROSE_ASSERT(n2->get_parent() != NULL);
                    ROSE_ASSERT(n2->get_file_info() != NULL);
                  }
             }
   };

// Rename all variables and labels in a block of code, and ensure that all
// variable and label references point to the correct declarations.
void renameVariables(SgNode* n) {
   std::vector<SgNode*> defs = NodeQuery::querySubTree(n, V_SgFunctionDefinition);
   for (size_t i = 0; i < defs.size(); ++i)
      {
        ROSE_ASSERT(defs[i]->get_parent() != NULL);
        RenameVariablesVisitor().traverse(defs[i]->get_parent(), preorder);
     }
}

// Visitor to rename all labels (and gotos to them) in a (possibly inlined)
// function definition
class RenameLabelsVisitor: public AstSimpleProcessing {
  static int labelRenameCounter;
  SgScopeStatement* newScope;
  SgSymbolTable* symtab;

  public:
  RenameLabelsVisitor(SgScopeStatement* scope):
    newScope(scope), symtab(scope->get_symbol_table()) {ROSE_ASSERT (symtab);}

  virtual void visit(SgNode* n) {
    SgLabelStatement* l = isSgLabelStatement(n);
    if (l) {
      SgName name = l->get_label();
      // The label is in some inner scope, and needs to be moved to the
      // enclosing function definition
      SgSymbolTable* st = l->get_scope()->get_symbol_table();
      ROSE_ASSERT (st);
      ROSE_ASSERT (st->find_label(name));
      st->remove(st->find_label(name));
      name << "__" << ++labelRenameCounter;
      // cout << "Found label " << l->get_label().getString() << " to rename to " << name.getString() << endl;
      l->set_label(name);
      l->set_scope(newScope);
      SgLabelSymbol* lSym = new SgLabelSymbol(l);
      lSym->set_parent(symtab);
      symtab->insert(name, lSym);
    }
  }
};

int RenameLabelsVisitor::labelRenameCounter = 0;

// Rename all labels in a (possibly inlined) function definition.  Gotos to
// them will be automatically updated as part of unparsing.
void renameLabels(SgNode* fd, SgFunctionDefinition* enclosingFunctionDefinition) {
#if 0
  SgNode* proj = enclosingFunctionDefinition;
  while (proj && !isSgProject(proj)) proj = proj->get_parent();
  ROSE_ASSERT (isSgProject(proj));
  generateAstGraph(isSgProject(proj), 400000);
#endif
  RenameLabelsVisitor(enclosingFunctionDefinition).traverse(fd, preorder);
}

// Remove all statements whose sole content is an integer constant.  Dummy
// statements like this are introduced by the inliner to ensure that a
// label is not the last statement in a block, but not all of them are
// necessary after unused labels are removed.
class RemoveNullStatementsVisitor: public AstSimpleProcessing {
  public:
  virtual void visit(SgNode* n) {
    if (isSgBasicBlock(n)) {
      SgBasicBlock* bb = isSgBasicBlock(n);
      SgStatementPtrList& stmts = bb->get_statements();
      bool changes = true;
      while (changes) {
        changes = false;
        for (SgStatementPtrList::iterator i = stmts.begin();
             i != stmts.end(); ++i) {
          if (isSgExprStatement(*i)) {
            SgExpression* expr = isSgExprStatement(*i)->get_expression();
            if (isSgIntVal(expr) ||
                isSgNullExpression(expr) ||
                isSgVarRefExp(expr) ||
                (isSgNotOp(expr) && isSgVarRefExp(isSgNotOp(expr)->get_operand()))) { // This is what null statements are
              SgStatementPtrList::iterator inext = i, iprev = i;
              ++inext;
              if (iprev != stmts.begin()) --iprev;
              if ((inext != stmts.end() && !isSgDeclarationStatement(*inext)) || // A label cannot precede a declaration
                  !isSgLabelStatement(*iprev)) {
                // Checking to be sure that this statement isn't ensuring
                // that a label isn't the last statement in a block (which
                // would be illegal)
                SageInterface::myRemoveStatement(*i);
                changes = true;
                break; // To avoid iterator invalidation
              }
            }
          }
        }
      }
    }
  }
};

// Count the number of references to a given variable (visitor).
class CountVariableReferencesVisitor: public AstSimpleProcessing {
  int& result;
  SgInitializedName* var;

  public:
  CountVariableReferencesVisitor(int& result, SgInitializedName* var):
    result(result), var(var) {}

  virtual void visit(SgNode* n) {
    if (isSgVarRefExp(n)) {
      assert (isSgVarRefExp(n)->get_symbol());
      if (isSgVarRefExp(n)->get_symbol()->get_declaration() == var)
        ++result;
    } else if (n == var) {
      ++result;
    }
  }
};

// Count references to a given variable
int countVariableReferences(SgNode* root, SgInitializedName* var) {
  int result = 0;
  CountVariableReferencesVisitor(result, var).traverse(root, preorder);
  return result;
}

// Does some code refer to a given variable?
bool containsVariableReference(SgNode* root, SgInitializedName* var) {
  return countVariableReferences(root, var) != 0;
}

// Does a given declaration declare a given variable?
bool isDeclarationOf(SgVariableDeclaration* decl, SgInitializedName* var) {
  SgInitializedNamePtrList& vars = decl->get_variables();
  for (SgInitializedNamePtrList::iterator i = vars.begin(); i != vars.end(); ++i)
    if (*i == var)
      return true;
  return false;
}

// Within sc, is the variable toCheck modified between the declaration of
// lifetime and its last use?  This is used to determine whether, whenever
// toCheck and lifetime are equal, one can be used as a substitute for the
// other.
bool isPotentiallyModifiedDuringLifeOf(SgBasicBlock* sc, 
                                       SgInitializedName* toCheck, 
                                       SgInitializedName* lifetime) {
  SgStatementPtrList& stmts = sc->get_statements();
  bool inLiveRange = false;
  for (SgStatementPtrList::reverse_iterator i = stmts.rbegin();
       i != stmts.rend(); ++i) {
    if (containsVariableReference(*i, lifetime))
      inLiveRange = true;
    SgVariableSymbol* toCheckSym = new SgVariableSymbol(toCheck);
    SgVarRefExp* toCheckVr = new SgVarRefExp(SgNULL_FILE, toCheckSym);

    bool result = false;
    if (inLiveRange && isPotentiallyModified(toCheckVr, *i)) {
      result = true;
    }
    delete toCheckSym;
    toCheckSym = NULL;
    toCheckVr->set_symbol(NULL);
    delete toCheckVr;
    toCheckVr = NULL;
    if (result) return true;

    if (isSgVariableDeclaration(*i) &&
        isDeclarationOf(isSgVariableDeclaration(*i), lifetime))
      return false; // This must be last
  }
  return false;
}

// Find all variables which are set as references to other variables and
// replace uses of the reference variable with what it is a reference to.
class FindReferenceVariablesVisitor: public AstSimpleProcessing {
  public:
  virtual void visit(SgNode* n) {
    if (isSgVarRefExp(n)) {
      SgVarRefExp* copy_vr = isSgVarRefExp(n);
      assert (copy_vr->get_symbol());
      SgInitializedName* copy = copy_vr->get_symbol()->get_declaration();
      assert (copy);
      if (!SageInterface::isReferenceType(copy->get_type()))
        return; // Fail if non-reference

      SgInitializer* copyinit = copy->get_initializer(); 
      SgNode* copyscope_ =
      copy->get_parent()->get_parent();
      while (!isSgScopeStatement(copyscope_))
          copyscope_ = copyscope_->get_parent();
      // cout << "copyscope is a " << copyscope_->sage_class_name() << endl;
      // SgScopeStatement* copyscope = isSgScopeStatement(copyscope_);
      if (isSgAssignInitializer(copyinit)) {
        SgAssignInitializer* init = isSgAssignInitializer(copyinit);
        SgExpression* orig_expr = init->get_operand();
        // cout << "orig is " << orig_expr->unparseToString() << ", copy is " << copy->get_name().str() << endl;
        bool shouldReplace = false;
        if (isSgVarRefExp(orig_expr)) {
          SgVarRefExp* orig_vr = isSgVarRefExp(orig_expr);
          // cout << "Found potential copy from " << orig_vr->get_symbol()->get_name().str() << " to " << copy_vr->get_symbol()->get_name().str() << endl;
          SgInitializedName* orig = orig_vr->get_symbol()->get_declaration();
          assert (orig);
#ifndef NDEBUG
          SgNode* origscope = orig->get_parent()->get_parent();
          assert (origscope);
#endif
          shouldReplace = true;
        }
        if (shouldReplace) {
          assert (orig_expr);
          SgExpression* orig_copy = 
            isSgExpression(orig_expr /*->copy(SgTreeCopy()) */);
          assert (orig_copy);
          orig_copy->set_parent(copy_vr->get_parent());
          isSgExpression(copy_vr->get_parent())->
            replace_expression(copy_vr, orig_copy);
        }
      }
    }
  }
};

// Find all (variable, value) and (variable, variable) pairs to which a
// simple form of copy and constant propagation can be applied.
class FindCopiesVisitor: public AstSimpleProcessing {
  public:
  virtual void visit(SgNode* n) {
    if (isSgVarRefExp(n)) {
      SgVarRefExp* copy_vr = isSgVarRefExp(n);
      assert (copy_vr->get_symbol());
      SgInitializedName* copy = copy_vr->get_symbol()->get_declaration();
      assert (copy);
      SgInitializer* copyinit = copy->get_initializer(); 
      SgScopeStatement* copyscope =
        SageInterface::getScope(copy->get_parent()->get_parent());
      if (isSgAssignInitializer(copyinit)) {
        SgAssignInitializer* init = 
          isSgAssignInitializer(copyinit);
        SgExpression* orig_expr = init->get_operand();
        // cout << "orig is " << orig_expr->unparseToString() << ", copy is " << copy->get_name().str() << endl;
        if (!isPotentiallyModified(copy_vr, copyscope) &&
            !isSgGlobal(copyscope) &&
            !isSgNamespaceDefinitionStatement(copyscope)) {
          bool shouldReplace = false;
          if (isSgVarRefExp(orig_expr)) {
            SgVarRefExp* orig_vr = isSgVarRefExp(orig_expr);
            // cout << "Found potential copy from " << orig_vr->get_symbol()->get_name().str() << " to " << copy_vr->get_symbol()->get_name().str() << endl;
            SgInitializedName* orig =
              orig_vr->get_symbol()->get_declaration();
            assert (orig);
            SgNode* origscope = orig->get_parent()->get_parent();
            assert (origscope);
            if (!hasAddressTaken(orig_vr, origscope) &&
                isSgBasicBlock(copyscope) &&
                !isPotentiallyModifiedDuringLifeOf(isSgBasicBlock(copyscope), 
                                                   orig, copy) &&
                !isSgGlobal(origscope) &&
                !isSgNamespaceDefinitionStatement(origscope)) {
              shouldReplace = true;
            }
          } else if (isSgValueExp(orig_expr)) {
            shouldReplace = true;
          }
          // cout << "shouldReplace is " << shouldReplace << endl;
          if (shouldReplace) {
            assert (orig_expr);
            SgExpression* orig_copy = isSgExpression(orig_expr /*->copy(SgTreeCopy()) */);
            assert (orig_copy);
            orig_copy->set_parent(copy_vr->get_parent());
            orig_copy->set_lvalue(copy_vr->get_lvalue());

       ROSE_ASSERT(copy_vr != NULL);
       ROSE_ASSERT(copy_vr->get_parent() != NULL);
       // ROSE_ASSERT(isSgExpression(copy_vr->get_parent()) != NULL);

       // DQ (12/15/2006): Need to handle cases where the parent is a SgStatement or a SgExpression (or make it an error).
       // isSgExpression(copy_vr->get_parent())->replace_expression(copy_vr, orig_copy);
       SgStatement* statement = isSgStatement(copy_vr->get_parent());
       if (statement != NULL)
          {
            statement->replace_expression(copy_vr, orig_copy);
          }
         else
          {
            SgExpression* expression = isSgExpression(copy_vr->get_parent());
            if (expression != NULL)
               {
                 expression->replace_expression(copy_vr, orig_copy);
               }
              else
               {
                 printf ("Error: what is this copy_vr->get_parent() = %s \n",copy_vr->get_parent()->class_name().c_str());
                 ROSE_ASSERT(false);
               }
          }

          }
        }
      }
    }
  }
};

// Find all used variable declarations.
class FindUsedDeclarationsVisitor: public AstSimpleProcessing {
  public:
  set<SgInitializedName*> used_decls;

  virtual void visit(SgNode* n) {
    if (isSgVarRefExp(n)) {
      assert (isSgVarRefExp(n)->get_symbol());
      used_decls.insert(isSgVarRefExp(n)->get_symbol()->get_declaration());
    }
  }
};

set<SgInitializedName*> findVariablesUsedInRegion(SgNode* e) {
  FindUsedDeclarationsVisitor vis;
  vis.traverse(e, preorder);
  return vis.used_decls;
}

// Remove unused variable declarations in which the initializer is null, a
// variable, or a constant.
class RemoveUnusedDeclarationsVisitor: public AstSimpleProcessing {
  const set<SgInitializedName*>& used_decls;
  const set<SgFunctionDeclaration*>& safe_functions;

  public:
  RemoveUnusedDeclarationsVisitor(const set<SgInitializedName*>& u,
                                  const set<SgFunctionDeclaration*>& s):
    used_decls(u), safe_functions(s) {}

  // Note: this now (11/7/2008) considers pointer derefs and array derefs safe,
  // which doesn't preserve invalid memory access behavior
  bool isSimpleInitializer(SgExpression* e) {
    if (isSgVarRefExp(e)) return true;
    if (isSgValueExp(e)) return true;
    if (isSgUnaryOp(e) && isSimpleInitializer(isSgUnaryOp(e)->get_operand())) {
      return isSgBitComplementOp(e) || isSgMinusOp(e) || isSgNotOp(e) || isSgUnaryAddOp(e) || isSgCastExp(e) || isSgPointerDerefExp(e);
    }
    if (isSgBinaryOp(e) && isSimpleInitializer(isSgBinaryOp(e)->get_lhs_operand()) && isSimpleInitializer(isSgBinaryOp(e)->get_rhs_operand())) {
      return isSgAddOp(e) || isSgAndOp(e) || isSgBitAndOp(e) || isSgBitOrOp(e) || isSgBitXorOp(e) || isSgCommaOpExp(e) || isSgDivideOp(e) || isSgEqualityOp(e) || isSgGreaterOrEqualOp(e) || isSgGreaterThanOp(e) || isSgLessOrEqualOp(e) || isSgLessThanOp(e) || isSgLshiftOp(e) || isSgModOp(e) || isSgMultiplyOp(e) || isSgNotEqualOp(e) || isSgOrOp(e) || isSgRshiftOp(e) || isSgSubtractOp(e) || isSgPntrArrRefExp(e);
    }
    if (isSgConditionalExp(e)) {
      SgConditionalExp* c = isSgConditionalExp(e);
      return isSimpleInitializer(c->get_conditional_exp()) && isSimpleInitializer(c->get_true_exp()) && isSimpleInitializer(c->get_false_exp());
    }
    if (isSgFunctionCallExp(e)) {
      SgFunctionRefExp* fr = isSgFunctionRefExp(isSgFunctionCallExp(e)->get_function());
      if (!fr) return false;
      SgFunctionDeclaration* decl = fr->get_symbol()->get_declaration();
      if (safe_functions.find(decl) == safe_functions.end()) {
        return false;
      }
      const SgExpressionPtrList& args = isSgFunctionCallExp(e)->get_args()->get_expressions();
      for (size_t i = 0; i < args.size(); ++i) {
        if (!isSimpleInitializer(args[i])) return false;
      }
      return true;
    }
    return false;
  }

  virtual void visit(SgNode* n) {
    if (isSgBasicBlock(n)) {
      SgBasicBlock* bb = isSgBasicBlock(n);
      bool changes1 = true;
      while (changes1) {
        changes1 = false;
        for (size_t i = 0; i < bb->get_statements().size(); ++i) {
          SgStatement* stmt = bb->get_statements()[i];
          if (isSgVariableDeclaration(stmt)) {
            SgInitializedNamePtrList& vars = 
              isSgVariableDeclaration(stmt)->get_variables();
            bool changes = true;
            while (changes) {
              changes = false;
              SgInitializedNamePtrList::iterator j;
              for (j = vars.begin(); j != vars.end(); ++j)
                if (used_decls.find(*j) == used_decls.end()) {
                  SgInitializer* init = (*j)->get_initializer();
                  bool shouldErase = false;
                  if (!init)
                    shouldErase = true;
                  else if (isSgAssignInitializer(init)) {
                    SgAssignInitializer* init2 = isSgAssignInitializer(init);
                    // Ensure that init does not have side effects
                    shouldErase = isSimpleInitializer(init2->get_operand());
                  }
                  if (shouldErase) {
                    removeVariableDeclaration(*j);
                    --i; // Counteract increment
                    goto iLoopBottom;
                    // changes = true;
                    // break;
                  }
                }
            }
#if 0
            if (vars.empty()) {
              bb->get_statements().erase(i);
              changes1 = true;
              break;
            }
#endif
          }
iLoopBottom: ;
        }
      }
    }
  }
};

// In code with declarations such as "int foo = bar", where foo and bar are
// not modified, replace "foo" with "bar" and remove the declaration
void simpleCopyAndConstantPropagation(SgNode* top) {
  FindReferenceVariablesVisitor().traverse(top, preorder);
  FindCopiesVisitor().traverse(top, preorder);
  FindUsedDeclarationsVisitor vis;
  vis.traverse(top, preorder);
  RemoveUnusedDeclarationsVisitor(vis.used_decls, set<SgFunctionDeclaration*>()).traverse(top, postorder);
}

// Remove unused variables in a scope
void removeUnusedVariables(SgNode* top, const set<SgFunctionDeclaration*>& safeFunctions) {
  set<SgInitializedName*> usedDecls;
  vector<SgNode*> varRefs = NodeQuery::querySubTree(top, V_SgVarRefExp);
  for (size_t i = 0; i < varRefs.size(); ++i) {
    usedDecls.insert(isSgVarRefExp(varRefs[i])->get_symbol()->get_declaration());
  }
  RemoveUnusedDeclarationsVisitor(usedDecls, safeFunctions).traverse(top, postorder);
}

// Remove the declaration of a given variable.
void removeVariableDeclaration(SgInitializedName* initname) {
  SgVariableDeclaration* parent = 
    isSgVariableDeclaration(initname->get_parent());
  assert (parent);
  SgInitializedNamePtrList& vars = parent->get_variables();
  SgInitializedNamePtrList::iterator i = vars.begin();
  for (; i != vars.end(); ++i)
    if (*i == initname)
      break;
  assert (i != vars.end());
#if 0
  vars.erase(i);
  if (vars.empty()) {
    myRemoveStatement(parent);
  }
#endif
  ROSE_ASSERT (vars.size() == 1);
  SageInterface::myRemoveStatement(parent);
}

// Move variable declarations down until just before they are used, and
// combine uninitialized variables with their first assignments if
// possible.
class MoveDeclarationsToFirstUseVisitor: public AstSimpleProcessing
   {
     public:
       virtual void visit(SgNode* n) {
         if (isSgBasicBlock(n)) {
           SgBasicBlock* bb = isSgBasicBlock(n);
           SgStatementPtrList& stmts = bb->get_statements();
           size_t initi;
           for (size_t decli = 0; decli < stmts.size(); ++decli) {
             if (isSgVariableDeclaration(stmts[decli])) {
               SgVariableDeclaration* decl = isSgVariableDeclaration(stmts[decli]);
               SgInitializedNamePtrList& vars = decl->get_variables();
               for (size_t vari = 0; vari != vars.size(); ++vari) {
                 SgInitializedName* in = vars[vari];
                 if (in->get_initializer() == 0) {
                   bool used = false;
                   for (initi = decli + 1; initi < stmts.size();
                        used |= containsVariableReference(stmts[initi], in),
                          ++initi) {
                     SgExprStatement* initExprStmt = isSgExprStatement(stmts[initi]);
                     if (initExprStmt) {
                       SgExpression* top = initExprStmt->get_expression();
                       if (isSgAssignOp(top)) {
                         SgVarRefExp* vr = isSgVarRefExp(isSgAssignOp(top)->get_lhs_operand());
                         ROSE_ASSERT(isSgAssignOp(top) != NULL);
                         SgExpression* newinit = isSgAssignOp(top)->get_rhs_operand();
                         if (!used && vr && vr->get_symbol()->get_declaration() == in) {
                           ROSE_ASSERT(newinit != NULL);
                           // printf ("MoveDeclarationsToFirstUseVisitor::visit(): newinit = %p = %s \n",newinit,newinit->class_name().c_str());
                           ROSE_ASSERT(newinit->get_type() != NULL);

                           SgAssignInitializer* i = new SgAssignInitializer(SgNULL_FILE,newinit,newinit->get_type());
                           i->set_endOfConstruct(SgNULL_FILE);
                           // printf ("Built a SgAssignInitializer #1 \n");

                        // DQ (4/6/2015): Adding testing of isTransformed flag (testing for consistancy).
                        // printf ("MoveDeclarationsToFirstUseVisitor: Testing 1: i = %p = %s testing: i->isTransformation() = %s \n",i,i->class_name().c_str(),i->isTransformation() ? "true" : "false");

                           vars[vari]->set_initializer(i);
                           stmts[initi] = decl;
                           newinit->set_parent(i);

                           // DQ (6/23/2006): Set the parent and file_info pointers
                           // printf ("Setting parent of i = %p = %s to parent = %p = %s \n",i,i->class_name().c_str(),in,in->class_name().c_str());
                           i->set_parent(in);
                           ROSE_ASSERT(i->get_parent() != NULL);

                        // DQ (4/6/2015): We should not need to build another Sg_File_Info object here becasue one was already build in the constructor.
                        // i->set_file_info(new Sg_File_Info(*(newinit->get_file_info())));
                           ROSE_ASSERT(i->get_file_info() != NULL);

                        // DQ (4/6/2015): Adding testing of isTransformed flag (testing for consistancy).
                        // printf ("MoveDeclarationsToFirstUseVisitor: Testing 2: i = %p = %s testing: i->isTransformation() = %s \n",i,i->class_name().c_str(),i->isTransformation() ? "true" : "false");

                           // Assumes only one var per declaration FIXME
                           ROSE_ASSERT (vars.size() == 1);
                           stmts.erase(stmts.begin() + decli);
                           --decli; // To counteract ++decli in loop header
                           break; // To get out of initi loop
                         }
                       }
                     }
                   }
                 }
               }
             }
           }
         }
       }
   };

bool inSimpleContext(SgExpression* expr) {
  if (!expr->get_parent())
    return false;
  if (!isSgExpression(expr->get_parent()))
    return true;
  if (!expr->get_parent()->get_parent())
    return false;
  if (isSgAssignOp(expr->get_parent()) && 
      !isSgExpression(expr->get_parent()->get_parent()) && 
      isSgVarRefExp(isSgAssignOp(expr->get_parent())->get_lhs_operand()))
    return true;
  if (isSgNotOp(expr->get_parent()) && 
      !isSgExpression(expr->get_parent()->get_parent()))
    return true;
  return false;
}

class SubexpressionExpansionVisitor: public AstSimpleProcessing {
  SgInitializedName* initname;
  SgExpression* initexpr;
  bool needSimpleContext;
  bool& changes;

  public:
  SubexpressionExpansionVisitor(SgInitializedName* initname,
                                SgExpression* initexpr,
                                bool needSimpleContext,
                                bool& changes):
    initname(initname), initexpr(initexpr), 
    needSimpleContext(needSimpleContext), changes(changes) {}

  virtual void visit(SgNode* n) {
    if (isSgVarRefExp(n)) {
      SgVarRefExp* vr = isSgVarRefExp(n);
      assert (vr->get_symbol());
      if (vr->get_symbol()->get_declaration() == initname) {
        if (inSimpleContext(vr) || !needSimpleContext) {
          SgTreeCopy tc;
          isSgExpression(n->get_parent())->replace_expression(
            vr, isSgExpression(initexpr->copy(tc)));
        }
      }
    }
  }
};

// Replaces all uses of a variable by its initialing expression.
// Requires that initname has an assign initializer
// Replaces all uses of initname in initname's scope by copy of its
//   initializer expression
// Then removes initname
void doSubexpressionExpansion(SgInitializedName* initname, 
                              bool needSimpleContext = false) {
  SgNode* root = initname->get_parent()->get_parent();
  assert (root);
  SgAssignInitializer* init;
  init = isSgAssignInitializer(initname->get_initializer());
  if (!init) return;
  bool changes = false;
  SubexpressionExpansionVisitor(initname, init, needSimpleContext, changes).
    traverse(root, postorder);
  if (changes)
    removeVariableDeclaration(initname);
}

// Same as doSubexpressionExpansion, but requires exactly one use of
// initname, and this use must be in a simple context
void doSubexpressionExpansionSmart(SgInitializedName* initname) {
  SgNode* root = initname->get_parent()->get_parent();
  assert (root);
  int count = countVariableReferences(root, initname);
  // cout << "Initname " << initname->get_name().str() << " was used " << count << " time(s)" << endl;
  if (count != 1) return;
  bool doExpansion = true;
  SgVariableSymbol* initnameSym = new SgVariableSymbol(initname);
  SgVarRefExp* initnameVr = new SgVarRefExp(SgNULL_FILE, initnameSym);
  if (isPotentiallyModified(initnameVr, root)) {
    doExpansion = false;
  }
  delete initnameSym;
  initnameSym = NULL;
  initnameVr->set_symbol(NULL);
  delete initnameVr;
  initnameVr = NULL;
  if (doExpansion) {
    doSubexpressionExpansion(initname, true);
  }
}

// Find all initialized names in some code.
class FindInitializedNames: public AstSimpleProcessing {
  public:
  SgInitializedNamePtrList ls;

  virtual void visit(SgNode* n) {
     if (isSgInitializedName(n))
     {
    // DQ (9/25/2007): Moved to use of std::vector instead of std::list uniformally in ROSE
    // ls.push_front(isSgInitializedName(n));
       ls.insert(ls.begin(),isSgInitializedName(n));
     }
  }
};

SgInitializedNamePtrList findInitializedNamesInScope(SgScopeStatement* scope) {
  FindInitializedNames fin;
  fin.traverse(scope, preorder);
  return fin.ls;
}

// Combined procedure for cleaning up code after inlining.  Does not do
// variable renaming or block flattening, however.
void cleanupInlinedCode(SgNode* top) 
   {
  // DQ (4/6/2015): Adding check for isTransformed flag consistancy.
  // SgGlobal* globalScope = TransformationSupport::getGlobalScope(top);
  // ROSE_ASSERT(globalScope != NULL);
     ROSE_ASSERT(isSgProject(top) != NULL);
     checkTransformedFlagsVisitor(top);

     simpleCopyAndConstantPropagation(top);
     SageInterface::removeJumpsToNextStatement(top);
     SageInterface::removeUnusedLabels(top);

  // DQ (4/6/2015): Adding check for isTransformed flag consistancy.
     checkTransformedFlagsVisitor(top);

     RemoveNullStatementsVisitor().traverse(top, postorder);

  // DQ (4/6/2015): Adding check for isTransformed flag consistancy.
     checkTransformedFlagsVisitor(top);

     MoveDeclarationsToFirstUseVisitor().traverse(top, postorder);

  // DQ (4/6/2015): Adding check for isTransformed flag consistancy.
     checkTransformedFlagsVisitor(top);

     FindInitializedNames fin;
     fin.traverse(top, preorder);
     for (SgInitializedNamePtrList::iterator i = fin.ls.begin(); i != fin.ls.end(); ++i) 
        {
          doSubexpressionExpansionSmart(*i);
        }

  // DQ (4/6/2015): Adding check for isTransformed flag consistancy.
     checkTransformedFlagsVisitor(top);

     simpleCopyAndConstantPropagation(top);
     RemoveNullStatementsVisitor().traverse(top, postorder);

    // Make sure the AST is consistent. To save time, we'll just fix things that we know can go wrong. For instance, the
    // SgAsmExpression.p_lvalue data member is required to be true for certain operators and is set to false in other
    // situations. Since we've introduced new expressions into the AST we need to adjust their p_lvalue according to the
    // operators where they were inserted.
     markLhsValues(top);

  // DQ (4/6/2015): Adding check for isTransformed flag consistancy.
     checkTransformedFlagsVisitor(top);
   }

void removeNullStatements(SgNode* top) {
  RemoveNullStatementsVisitor().traverse(top, postorder);
}

class ChangeAllMembersToPublicVisitor: public AstSimpleProcessing {
  public:
  virtual void visit(SgNode* n) {
    if (isSgDeclarationStatement(n)) {
      SgDeclarationStatement* n2 = isSgDeclarationStatement(n);
      SgDeclarationModifier& dm = n2->get_declarationModifier();
      SgAccessModifier& am = dm.get_accessModifier();
      if (am.isPrivate() || am.isProtected()) {
        am.setPublic();
      }
    }
  }
};

// Change all members in a program to be public.  There should really be a
// smarter procedure for this that only changes members used by inlined
// code.
void changeAllMembersToPublic(SgNode* top) {
  ChangeAllMembersToPublicVisitor().traverse(top, preorder);
}


// Find all used variable declarations.
class CheckTransformedFlagsVisitor: public AstSimpleProcessing 
   {
     public:
          virtual void visit(SgNode* n) 
             {
               SgLocatedNode* locatedNode = isSgLocatedNode(n);
               if (locatedNode != NULL)
                  {
#if 0
                    printf ("CheckTransformedFlagsVisitor: Found locatedNode = %p = %s \n",locatedNode,locatedNode->class_name().c_str());
#endif
                    if (locatedNode->isTransformation() == true)
                       {
#if 0
                         printf ("Found locatedNode = %p = %s as transformation \n",locatedNode,locatedNode->class_name().c_str());
                      // ROSE_ASSERT(false);
#endif
                       }
                  }
             }
   };

void checkTransformedFlagsVisitor(SgNode* e) 
   {
     CheckTransformedFlagsVisitor traversal;
     traversal.traverse(e, preorder);
   }


#include "rose.h"

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;

SgGlobal* globalScope;

SgFunctionSymbol* getFunction(const string& name) {
  SgFunctionSymbol* sym = lookupFunctionSymbolInParentScopes(name, globalScope);
  return sym;
}

SgUnaryOp::Sgop_mode invertOpMode(SgUnaryOp::Sgop_mode m) {
  switch (m) {
    case SgUnaryOp::prefix: return SgUnaryOp::postfix;
    case SgUnaryOp::postfix: return SgUnaryOp::prefix;
    default: return m;
  }
}

SgExpression* makeStatePush(SgVariableSymbol* st, SgExpression* expr) {
  SgExprListExp* args = buildExprListExp();
  appendExpression(args, buildVarRefExp(st));
  appendExpression(args, expr);
  SgExpression* f = buildFunctionCallExp(getFunction("statePush"), args);
  return f;
}

SgExpression* makeStatePop(SgVariableSymbol* st) {
  SgExprListExp* args = buildExprListExp();
  appendExpression(args, buildVarRefExp(st));
  SgExpression* f = buildFunctionCallExp(getFunction("statePop"), args);
  return f;
}

void reverseOneFunction(SgFunctionDeclaration* decl, SgFunctionSymbol*& forward, SgFunctionSymbol*& backward);

void reverseOneStatement(SgStatement* stmt, SgBasicBlock* forwardBlockToAppendTo, SgVariableSymbol* forwardSaveStack, SgBasicBlock* backwardBlockToPrependTo, SgVariableSymbol* backwardSaveStack) {
  switch (stmt->variantT()) {
    case V_SgBasicBlock: {
      SgBasicBlock* bb = isSgBasicBlock(stmt);
      for (size_t i = 0; i < bb->get_statements().size(); ++i) {
        reverseOneStatement(bb->get_statements()[i], forwardBlockToAppendTo, forwardSaveStack, backwardBlockToPrependTo, backwardSaveStack);
      }
      break;
    }
    case V_SgExprStatement: {
      SgExpression* expr = isSgExprStatement(stmt)->get_expression();
      // Assumption: the only mutating operation is at the top level of the expr
      SgExpression* forwardExpr = NULL;
      SgExpression* backwardExpr = NULL;
      switch (expr->variantT()) {
        case V_SgFunctionCallExp: {
          SgFunctionCallExp* fc = isSgFunctionCallExp(expr);
          ROSE_ASSERT (fc);
          SgFunctionRefExp* fr = isSgFunctionRefExp(fc->get_function());
          ROSE_ASSERT (fr);
          SgFunctionDeclaration* decl = fr->get_symbol()->get_declaration();
          ROSE_ASSERT (decl);
          SgFunctionSymbol* forwardSym = NULL;
          SgFunctionSymbol* backwardSym = NULL;
          reverseOneFunction(decl, forwardSym, backwardSym);
          ROSE_ASSERT (forwardSym && backwardSym);
          SgExprListExp* argsForward = deepCopy(fc->get_args());
          appendExpression(argsForward, buildVarRefExp(forwardSaveStack));
          SgExprListExp* argsBackward = deepCopy(fc->get_args());
          appendExpression(argsBackward, buildVarRefExp(backwardSaveStack));
          forwardExpr = buildFunctionCallExp(forwardSym, argsForward);
          backwardExpr = buildFunctionCallExp(backwardSym, argsBackward);
          break;
        }
        case V_SgPlusPlusOp: {
          forwardExpr = copyExpression(expr);
          backwardExpr = buildMinusMinusOp(copyExpression(isSgUnaryOp(expr)->get_operand()), invertOpMode(isSgUnaryOp(expr)->get_mode()));
          break;
        }
        case V_SgMinusMinusOp: {
          forwardExpr = copyExpression(expr);
          backwardExpr = buildPlusPlusOp(copyExpression(isSgUnaryOp(expr)->get_operand()), invertOpMode(isSgUnaryOp(expr)->get_mode()));
          break;
        }
        case V_SgAssignOp: {
          appendStatement(buildExprStatement(makeStatePush(forwardSaveStack, copyExpression(isSgAssignOp(expr)->get_lhs_operand()))), forwardBlockToAppendTo);
          forwardExpr = copyExpression(expr);
          backwardExpr = buildAssignOp(copyExpression(isSgAssignOp(expr)->get_lhs_operand()), makeStatePop(backwardSaveStack));
          break;
        }
        default: {
          cerr << "Can't reverse mutating expression " << expr->class_name() << endl;
          ROSE_ASSERT (false);
        }
      }
      ROSE_ASSERT (forwardExpr && backwardExpr);
      appendStatement(buildExprStatement(forwardExpr), forwardBlockToAppendTo);
      prependStatement(buildExprStatement(backwardExpr), backwardBlockToPrependTo);
      break;
    }
    default: {
      cerr << "Don't know how to reverse statement of type " << stmt->class_name() << endl;
      ROSE_ASSERT (false);
    }
  }
}

void reverseOneFunction(SgFunctionDeclaration* decl, SgFunctionSymbol*& forward, SgFunctionSymbol*& backward) {
  ROSE_ASSERT (decl);
  string name = decl->get_name().getString();
  forward = getFunction(name + "__forward");
  backward = getFunction(name + "__backward");
  if (forward && backward) return; // Already have reversal
  ROSE_ASSERT (!forward && !backward); // Should not have just one of the two
  SgFunctionParameterList* paramList1 = deepCopy(decl->get_parameterList());
  ROSE_ASSERT (paramList1);
  SgVariableSymbol* forwardSaveStack = appendArg(paramList1, buildInitializedName("__reversalSaveStack", buildPointerType(buildCharType())));
  SgFunctionParameterList* paramList2 = deepCopy(decl->get_parameterList());
  ROSE_ASSERT (paramList2);
  SgVariableSymbol* backwardSaveStack = appendArg(paramList2, buildInitializedName("__reversalSaveStack", buildPointerType(buildCharType())));
  SgFunctionDeclaration* forwardDecl = buildDefiningFunctionDeclaration(name + "__forward", SgTypeVoid::createType(), paramList1, globalScope);
  ROSE_ASSERT (forwardDecl);
  SgFunctionDeclaration* backwardDecl = buildDefiningFunctionDeclaration(name + "__backward", SgTypeVoid::createType(), paramList2, globalScope);
  ROSE_ASSERT (backwardDecl);
  appendStatement(forwardDecl, globalScope);
  appendStatement(backwardDecl, globalScope);
  forward = getFunction(name + "__forward");
  backward = getFunction(name + "__backward");
  ROSE_ASSERT (forward && backward);
  SgFunctionDefinition* forwardDefn = forwardDecl->get_definition();
  ROSE_ASSERT (forwardDefn);
  forwardDefn->insert_symbol(forwardSaveStack->get_name(), forwardSaveStack);
  forwardSaveStack->get_declaration()->set_scope(forwardDefn);
  SgFunctionDefinition* backwardDefn = backwardDecl->get_definition();
  ROSE_ASSERT (backwardDefn);
  backwardDefn->insert_symbol(backwardSaveStack->get_name(), backwardSaveStack);
  backwardSaveStack->get_declaration()->set_scope(backwardDefn);
  decl = isSgFunctionDeclaration(decl->get_definingDeclaration());
  ROSE_ASSERT (decl);
  SgFunctionDefinition* defn = decl->get_definition();
  ROSE_ASSERT (defn);
  reverseOneStatement(defn->get_body(), forwardDefn->get_body(), forwardSaveStack, backwardDefn->get_body(), backwardSaveStack);
  return;
}

int main(int argc, char * argv[]) {
  SgProject* project = frontend(argc, argv);
  AstTests::runAllTests(project);
  ROSE_ASSERT (project->get_fileList()->size() == 1);
  SgFile* f = (*project->get_fileList())[0];
  ROSE_ASSERT (f);
  SgGlobal* g = f->get_globalScope();
  ROSE_ASSERT (g);
  globalScope = g;
  SgFunctionSymbol* sym = getFunction("processEvent");
  ROSE_ASSERT (sym);
  SgFunctionSymbol* forward = NULL;
  SgFunctionSymbol* backward = NULL;
  reverseOneFunction(sym->get_declaration(), forward, backward);
  ROSE_ASSERT (forward && backward);
  return backend(project);
}


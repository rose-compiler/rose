#include <string>
#include <iostream>
#include "rose.h"

using namespace std;
using namespace SageBuilder;
using namespace SageInterface;

// Types, Globals //////////////////////////////////////////////////////////////

typedef enum {i_max, i_min, i_const} IterationType;

static bool debug = 0;

// Utilities ///////////////////////////////////////////////////////////////////


void exitWithMsg (SgStatement *stmt, const char *pragma_arg, const char *msg) {
  fprintf (stderr,
           "ERROR: In pragma '#pragma skel %s':\n  %s\n",
           pragma_arg,
           msg);

  Sg_File_Info &fileInfo = *(stmt->get_file_info());
  fprintf (stderr, "  (File %s, line %d, column %d)\n",
           fileInfo.get_filename(),
           fileInfo.get_line(),
           fileInfo.get_col());
  exit (1);
}

// loop pragma /////////////////////////////////////////////////////////////////

SgExpression *modifyTestExpr (SgStatement *stmt,
                              SgStatement *origtest0,
                              SgExpression *count,
                              IterationType it,
                              SgVarRefExp *kref)
{
  SgExprStatement *origtest1 = isSgExprStatement(origtest0);
  if (!origtest1) {
    exitWithMsg(stmt,
         "loop", "does not support loops if test is a statement");
    return NULL;  // FIXME
  }
  SgExpression *origtest = origtest1->get_expression();

  SgExpression *testk = buildLessThanOp(kref,count);
  switch (it) {
    case i_min   : return buildOrOp (origtest,testk);       break;
    case i_max   : return buildAndOp (origtest,testk);      break;
    case i_const : return buildCommaOpExp (origtest,testk); break;
    default      : return NULL;                             break;
  }
}

void loopIterate (SgStatement *statement, SgExpression *count, IterationType it) {
  SgForStatement  *stmt_for   = isSgForStatement(statement);
  SgDoWhileStmt   *stmt_do    = isSgDoWhileStmt(statement);
  SgWhileStmt     *stmt_while = isSgWhileStmt(statement);

  // Common code for for/do-while/while:
  SgScopeStatement *ss = getScope(statement);
  assert(ss);
  pushScopeStack(ss);
  SgStatement *stmtNew;

  // - Create new, unique name [hopefully--don't transform your code twice]:
  SgName knm = "_k_";
  knm << ++gensym_counter;

  // - Create "unsigned long k = 0":
  SgVariableDeclaration *kDecl =
    buildVariableDeclaration ( knm,
                               buildUnsignedLongType (),
                               buildAssignInitializer(buildUnsignedLongVal(0)));
  SgVarRefExp *kref = buildVarRefExp(knm);
  SgExpression *incrementk = buildPlusPlusOp(kref, SgUnaryOp::postfix);

  if (stmt_for) {
    // for /////////////////////////////////////////////////////////////////////
    // Create the new test:
    SgExpression *newtest =
      modifyTestExpr(statement, stmt_for->get_test(), count, it, kref);
    // Create the new For Loop:
    moveForStatementIncrementIntoBody (stmt_for);
      // NOTE: handles continue's properly!
    stmtNew =
        buildForStatement( buildForInitStatement (stmt_for->get_init_stmt()),
                           buildExprStatement(newtest),
                           incrementk,    // k++
                           stmt_for->get_loop_body(),
                           NULL);  // TODO: What is this anyway?
  }
  else if (stmt_do) {
    // doWhile /////////////////////////////////////////////////////////////////
    SgStatement *body = stmt_do->get_body();
    SgStatement *cond = stmt_do->get_condition();
    stmtNew =
      buildDoWhileStmt( buildBasicBlock(body, buildExprStatement(incrementk)),
                        modifyTestExpr(statement, cond, count, it, kref));
  }
  else if (stmt_while) {
    // while ///////////////////////////////////////////////////////////////////
    SgStatement *body = stmt_while->get_body();
    SgStatement *cond = stmt_while->get_condition();
    stmtNew =
      buildWhileStmt( modifyTestExpr(statement, cond, count, it, kref),
                      buildBasicBlock(body, buildExprStatement(incrementk)));
  }
  else
    exitWithMsg (statement,
                 "loop",
                 "must be followed by for(;;){},  do{}while(),  or while(){}");

  // Common code for for/do-while/while:

  replaceStatement(statement,
                   buildBasicBlock(kDecl,stmtNew),
                   1);

  if (debug)
    cout << "  target (transformed):\n"
         << "    " << kDecl->unparseToString() << endl
         << "    " << stmtNew->unparseToString() << endl << endl;

  popScopeStack();
}

// condition pragma ////////////////////////////////////////////////////////////

void addStdlibH (const SgNode *n) {
  insertHeader ( "stdlib.h",
                   // to include RAND_MAX and rand() declaration.
                 PreprocessingInfo::after,
                 true,
                 getGlobalScope(n));
}

void conditionProbability (SgStatement *statement, SgExpression *x) {
  // ISSUES: Q. if stdlib.h not included?

  SgIfStmt *stmt = isSgIfStmt(statement);
  if (!stmt)
    exitWithMsg(statement, "condition",  "must be followed by if statement");

  SgScopeStatement *ss = getScope(stmt);
  assert(ss);
  pushScopeStack(ss);

  // Create the new test:
  SgExprStatement *test = isSgExprStatement(stmt->get_conditional());
  if (!test)
    exitWithMsg(statement,
                "condition",
                "does not support 'if(t) ...' when 't' is a statement");

  SgVarRefExp *rand_max = buildOpaqueVarRefExp("RAND_MAX");
  SgName randfunc("rand");

  SgExpression *newtest =
    buildCommaOpExp( test->get_expression(),
                     buildLessThanOp(
                       buildDivideOp(
                         buildCastExp(buildFunctionCallExp(
                                        randfunc,buildIntType(),NULL),
                                      buildDoubleType()),
                         rand_max),
                       x));

  SgIfStmt *stmtNew =
    buildIfStmt( newtest,
                 stmt->get_true_body(),
                 stmt->get_false_body());

  replaceStatement(stmt, stmtNew, 1);

  addStdlibH(stmtNew);
  popScopeStack();

  if (debug)
    cout << "  target (transformed):\n"
         << "    " << stmt->unparseToString() << endl << endl;
}

// initializer pragma //////////////////////////////////////////////////////////

void arrayInitializer (SgStatement *statement, SgExpression *x) {

  SgVariableDeclaration *stmt = isSgVariableDeclaration(statement);
  if (!stmt)
    exitWithMsg(statement,
                "initializer",
                "must be followed by declaration of array");

  SgScopeStatement *ss = getScope(stmt);
  assert(ss);
  pushScopeStack(ss);

  const SgInitializedNamePtrList &vars = stmt->get_variables();
  SgInitializedName *name = vars.front();
    // FIXME: make robust.
    // just allow one declaration after pragma.

  const SgType *type        = name->get_type();
  const SgInitializer *init = name->get_initializer();
  const SgArrayType *at     = isSgArrayType(type);

  if (init != NULL || at == NULL)
    exitWithMsg(
      statement,
      "initializer",
      "must be followed by declaration of array without initializers");

  // const SgType *bt    = at->get_base_type();
    // TODO: support nested arrays.
  SgExpression *index = at->get_index();

  // FIXME: error if index has side-effect: need to create variable for it.

  /*
  Static Arrays?
    // TODO: Way to determine if index evals to constant expr?  [For static arrays]
    SgUnsignedLongVal *i = isSgUnsignedLongVal(index);
    // Get length of list: (FIXME: Ad hoc!)
    if (!i)
      exitWithMsg(statement,
                  "initializer",
                  "only supports arrays of constant length");
    unsigned long int ival = i->get_value();
    printf ("size = %ld \n",ival);
  */

  // New name:
  SgName knm = "_k_";
  knm << ++gensym_counter;

  // Make "int k = 0":
  SgVariableDeclaration *kDecl =
    buildVariableDeclaration ( knm,
                               buildUnsignedLongType (),
                               buildAssignInitializer(buildUnsignedLongVal(0)));
  SgVarRefExp *kref = buildVarRefExp(knm);

  // Add the initializing loop:
  SgForStatement *forStmt =
    buildForStatement( buildNullStatement(),
                       buildExprStatement(buildLessThanOp(kref,index)),
                       buildPlusPlusOp(kref,SgUnaryOp::postfix),    // k++
                       buildAssignStatement (buildPntrArrRefExp(buildVarRefExp(name),kref),
                                             x),
                       NULL);  // TODO: What is this anyway?

  insertStatementAfter (stmt, kDecl);
  insertStatementAfter (kDecl, forStmt);

  popScopeStack();
  if (debug)
    cout << "  target (transformed):\n"
         << "    " << stmt->unparseToString() << endl << endl;
}

// Library Functions ///////////////////////////////////////////////////////////

int match (const char *input, const char *prefix) {
  int i = 0;

  // create sscanf format string in 's':
  static const char fmt[] = "%n";
  char s[strlen(prefix)+strlen(fmt)];

  strcpy(s,prefix);
  strcat(s,fmt);

  sscanf(input, s, &i);
  return i;
}

bool supportedFileType (SgFile *f) {

  const string s = f->getFileName();
  bool cfile = s.find(".c", s.length() - 2) != string::npos;
  return (cfile || f->get_sourceFileUsesCppFileExtension());

  // FIXME: Ad hoc!  What's the recommended way to test for C?
}

// parseExpr - parse an expression surrounded by parentheses.
// ISSUE:
//  - due to the lack of support in ROSE, it does not support floating literals

SgExpression *parseExpr (SgStatement *context, const char *s) {

  if (debug) printf ("parseExpr: %s \n", s);

  AstFromString::c_char = s;       // parse this
  AstFromString::c_sgnode = context;  // sets up scope & such

  if (!AstFromString::afs_match_char('(')) {
    printf ("Error: expecting '(' after parsing #pragma skel ...\n");
    return NULL;
  }

  if (! AstFromString::afs_match_assignment_expression())
    return NULL;

  if (! AstFromString::afs_match_char(')')) {
    printf ("Error: expecting ')' after parsing #pragma skel ...\n");
    return NULL;
  }

  SgExpression *e = isSgExpression(AstFromString::c_parsed_node);
  if (debug && e)
    cout << "expr= " << e->unparseToString() << endl;
  return e;
}

// pragma parse & process //////////////////////////////////////////////////////

void process1pragma(SgPragmaDeclaration *p) {
  string pragmaText = p->get_pragma()->get_pragma();
  SgStatement *stmt = SageInterface::getNextStatement((SgStatement*) p);

  const char *s = pragmaText.c_str();

  if (debug) printf("visit: %s\n", s);

  int i = 0;
  sscanf (s, "skel %n", &i);
  if (i > 0) { // i.e., sscanf made progress.
    static const char parseErrorMsg[] = "error in parsing expression";

    s += i;    // point past "skel"
    if (debug)
      cout << "#pragma skel:\n"
           << "  parameter:\n"
           << "   " << s << endl
           << "  target:\n"
           << "    " << stmt->unparseToString()
           << endl;

    int j;     // store result of match.
    if ((j = match(s,"loop iterate atmost")) != 0) {
      SgExpression *e = parseExpr (p, s+j);
      if (!e)
        exitWithMsg(stmt, s, parseErrorMsg);
      else
        loopIterate (stmt, e, i_max);
    } else if ((j = match(s,"loop iterate atleast")) != 0) {
      SgExpression *e = parseExpr (p, s+j);
      if (!e)
        exitWithMsg(stmt, s, parseErrorMsg);
      else
        loopIterate (stmt, e, i_min);
    } else if ((j = match(s,"loop iterate exactly")) != 0) {
      SgExpression *e = parseExpr (p, s+j);
      if (!e)
        exitWithMsg(stmt, s, parseErrorMsg);
      else
        loopIterate (stmt, e, i_const);
    } else if ((j = match(s,"condition prob")) != 0) {
      SgExpression *e = parseExpr (p, s+j);
      if (!e)
        exitWithMsg(stmt, s, parseErrorMsg);
      else
        conditionProbability (stmt, e);
    } else if ((j = match(s,"initializer repeat")) != 0) {
      SgExpression *e = parseExpr (p, s+j);
      if (!e)
        exitWithMsg(stmt, s, parseErrorMsg);
      else
        arrayInitializer (stmt, e);
    } else {
        exitWithMsg (stmt, s, "unrecognized arguments");
    }
  }
}

// Traversal to gather all pragmas /////////////////////////////////////////////

class CollectPragmaTargets : public AstSimpleProcessing
{
public:
  // Container of list statements, in order.
  typedef list<SgPragmaDeclaration *> TgtList_t;

  // Call this routine to gather the pragmas.
  static void collect (SgProject* p, TgtList_t& final)
  {
    CollectPragmaTargets collector (final);
    collector.traverseInputFiles (p, postorder);
  }

  virtual void visit (SgNode* n)
  {
    SgPragmaDeclaration* s = isSgPragmaDeclaration(n);
    if (s)
      final_targets_.push_back (s);
  }

private:
  CollectPragmaTargets (TgtList_t& final) : final_targets_ (final) {}
  TgtList_t& final_targets_; // Final list of targets.
};


// Main entry point ////////////////////////////////////////////////////////////

void processPragmas (SgProject *project) {

  // Check that file types are supported:
  for (int i=0; i < project->numberOfFiles(); i++)
    if (!supportedFileType((*project)[i])) {
      printf("ERROR: Only C and C++ files are supported.\n");
      exit(1);
    }

  // Build a set of pragma targets.
  CollectPragmaTargets::TgtList_t ts;
  CollectPragmaTargets::collect (project, ts);

  // Process them all.
  for (CollectPragmaTargets::TgtList_t::iterator i = ts.begin ();
       i != ts.end ();
       ++i)
    process1pragma(*i);
}

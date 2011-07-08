#ifndef UNPARSER_PYTHON_H_
#define UNPARSER_PYTHON_H_

#include "unparser.h"

class SgStatement;
class SgBasicBlock;
class SgClassDefinition;
class SgNamespaceDefinitionStatement;

#include "unparseLanguageIndependentConstructs.h"

class Unparse_Python : public UnparseLanguageIndependentConstructs
   {
     public:
          Unparse_Python(Unparser* unp, std::string fname);
          virtual ~Unparse_Python();

          /* SageIII requires all statements in the global scope to be declaration
           * statements. Python allows arbitrary expressions, so we've wrapped the
           * entire program in an implicit __main__() function. When unparsing, we
           * don't want to to appear, so this function only unparses the body of the
           * wrapper function.
           */
          virtual void unparseWrappedProgram(SgScopeStatement*, SgUnparse_Info&);

          virtual void unparseLanguageSpecificStatement(SgStatement*, SgUnparse_Info&);
          virtual void unparseLanguageSpecificExpression(SgExpression*, SgUnparse_Info&);
          virtual void unparseStringVal(SgExpression*, SgUnparse_Info&);
          virtual void unparseComplexVal(SgExpression* expr, SgUnparse_Info& info);

       // DQ (9/6/2010): Mark the derived class to support debugging.
          virtual std::string languageName() const { return "Python Unparser"; }

     protected:
          virtual void unparseAssignOp(SgAssignOp*, SgUnparse_Info&);
          virtual void unparseAssignInitializer(SgAssignInitializer*, SgUnparse_Info&);
          virtual void unparseBasicBlock(SgBasicBlock*, SgUnparse_Info&);
          virtual void unparseBinaryOp(SgBinaryOp*, SgUnparse_Info&);
          virtual void unparseComplexVal(SgComplexVal*, SgUnparse_Info&);
          virtual void unparseExprStatement(SgExprStatement*, SgUnparse_Info&);
          virtual void unparseExprListExp(SgExprListExp*, SgUnparse_Info&);
          virtual void unparseFunctionCallExp(SgFunctionCallExp*, SgUnparse_Info&);
          virtual void unparseFunctionDeclaration(SgFunctionDeclaration*, SgUnparse_Info&);
          virtual void unparseFunctionDefinition(SgFunctionDefinition*, SgUnparse_Info&);
          virtual void unparseFunctionParameterList(SgFunctionParameterList*, SgUnparse_Info&);
          virtual void unparseIfStmt(SgIfStmt*, SgUnparse_Info&);
          virtual void unparseInitializedName(SgInitializedName*, SgUnparse_Info&);
          virtual void unparseLambdaRefExp(SgLambdaRefExp*, SgUnparse_Info&);
          virtual void unparseListExp(SgListExp*, SgUnparse_Info&);
          virtual void unparseLongIntVal(SgLongIntVal*, SgUnparse_Info&);
          virtual void unparsePythonPrintStmt(SgPythonPrintStmt*, SgUnparse_Info&);
          virtual void unparseReturnStmt(SgReturnStmt*, SgUnparse_Info&);
          virtual void unparseStringVal(SgStringVal*, SgUnparse_Info&);
          virtual void unparseTupleExp(SgTupleExp*, SgUnparse_Info&);
          virtual void unparseUnaryOp(SgUnaryOp*, SgUnparse_Info&);
          virtual void unparseVarRefExp(SgVarRefExp*, SgUnparse_Info&);
          virtual void unparseWhileStmt(SgWhileStmt*, SgUnparse_Info&);

          virtual std::string ws_prefix(int nesting_level);
   };

#define ROSE_PYTHON_WRAPPER_FXN_NAME "__main__"

#define ROSE_PYTHON_ADD_OP      "+"
#define ROSE_PYTHON_ASSIGN_OP   "="
#define ROSE_PYTHON_BITAND_OP   "&"
#define ROSE_PYTHON_BITOR_OP    "|"
#define ROSE_PYTHON_BITXOR_OP   "^"
#define ROSE_PYTHON_DIV_OP      "/"
#define ROSE_PYTHON_EXP_OP      "**"
#define ROSE_PYTHON_IDIV_OP     "//"
#define ROSE_PYTHON_LSHIFT_OP   "<<"
#define ROSE_PYTHON_RSHIFT_OP   ">>"
#define ROSE_PYTHON_MOD_OP      "%"
#define ROSE_PYTHON_MULT_OP     "*"
#define ROSE_PYTHON_SUB_OP      "-"

#define ROSE_PYTHON_AUG_ADD_OP      "+="
#define ROSE_PYTHON_AUG_BITAND_OP   "&="
#define ROSE_PYTHON_AUG_BITOR_OP    "|="
#define ROSE_PYTHON_AUG_BITXOR_OP   "^="
#define ROSE_PYTHON_AUG_DIV_OP      "/="
#define ROSE_PYTHON_AUG_EXP_OP      "**="
#define ROSE_PYTHON_AUG_IDIV_OP     "//="
#define ROSE_PYTHON_AUG_LSHIFT_OP   "<<="
#define ROSE_PYTHON_AUG_RSHIFT_OP   ">>="
#define ROSE_PYTHON_AUG_MOD_OP      "%="
#define ROSE_PYTHON_AUG_MULT_OP     "*="
#define ROSE_PYTHON_AUG_SUB_OP      "-="

#define ROSE_PYTHON_EQ_OP   "=="
#define ROSE_PYTHON_NE_OP   "!="
#define ROSE_PYTHON_LE_OP   "<="
#define ROSE_PYTHON_GE_OP   ">="
#define ROSE_PYTHON_LT_OP   "<"
#define ROSE_PYTHON_GT_OP   ">"
#define ROSE_PYTHON_IS_OP      "is"
#define ROSE_PYTHON_ISNOT_OP   "is not"
#define ROSE_PYTHON_IN_OP      "in"
#define ROSE_PYTHON_NOTIN_OP   "not in"

#define ROSE_PYTHON_AND_OP  "and"
#define ROSE_PYTHON_OR_OP   "or"
#define ROSE_PYTHON_NOT_OP  "not"

#define ROSE_PYTHON_UADD_OP   "+"
#define ROSE_PYTHON_USUB_OP   "-"
#define ROSE_PYTHON_INVERT_OP "~"

#endif /* UNPARSER_PYTHON_H_ */




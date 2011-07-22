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

          // Override the default set of operator precedences
          virtual int getPrecedence(int variant);
          virtual int getAssociativity(int variant);

          virtual void unparseLanguageSpecificStatement(SgStatement*, SgUnparse_Info&);
          virtual void unparseLanguageSpecificExpression(SgExpression*, SgUnparse_Info&);
          virtual void unparseExpression(SgExpression*, SgUnparse_Info&);
          virtual void unparseGlobalStmt(SgStatement*, SgUnparse_Info&);
          virtual void unparseStringVal(SgExpression*, SgUnparse_Info&);
          virtual void unparseComplexVal(SgExpression* expr, SgUnparse_Info& info);

       // DQ (9/6/2010): Mark the derived class to support debugging.
          virtual std::string languageName() const { return "Python Unparser"; }

          virtual bool requiresParentheses(SgExpression* expr);
          virtual void curprint_indented(std::string txt, SgUnparse_Info& info);

     protected:
          virtual void unparseAssertStmt(SgAssertStmt*, SgUnparse_Info&);
          virtual void unparseAssignOp(SgAssignOp*, SgUnparse_Info&);
          virtual void unparseAssignInitializer(SgAssignInitializer*, SgUnparse_Info&);
          virtual void unparseBasicBlock(SgBasicBlock*, SgUnparse_Info&);
          virtual void unparseBinaryOp(SgBinaryOp*, SgUnparse_Info&);
          virtual void unparseBreakStmt(SgBreakStmt*, SgUnparse_Info&);
          virtual void unparseCatchOptionStmt(SgCatchOptionStmt*, SgUnparse_Info&);
          virtual void unparseClassDeclaration(SgClassDeclaration*, SgUnparse_Info&);
          virtual void unparseClassDefinition(SgClassDefinition*, SgUnparse_Info&);
          virtual void unparseComplexVal(SgComplexVal*, SgUnparse_Info&);
          virtual void unparseComprehension(SgComprehension*, SgUnparse_Info&);
          virtual void unparseContinueStmt(SgContinueStmt*, SgUnparse_Info&);
          virtual void unparseDeleteExp(SgDeleteExp*, SgUnparse_Info&);
          virtual void unparseDictionaryComprehension(SgDictionaryComprehension*, SgUnparse_Info&);
          virtual void unparseExprStatement(SgExprStatement*, SgUnparse_Info&);
          virtual void unparseExprListExp(SgExprListExp*, SgUnparse_Info&);
          virtual void unparseFunctionCallExp(SgFunctionCallExp*, SgUnparse_Info&);
          virtual void unparseFunctionDeclaration(SgFunctionDeclaration*, SgUnparse_Info&);
          virtual void unparseFunctionDefinition(SgFunctionDefinition*, SgUnparse_Info&);
          virtual void unparseFunctionParameterList(SgFunctionParameterList*, SgUnparse_Info&);
          virtual void unparseFunctionRefExp(SgFunctionRefExp*, SgUnparse_Info&);
          virtual void unparseForInitStatement(SgForInitStatement*, SgUnparse_Info&);
          virtual void unparseForStatement(SgForStatement*, SgUnparse_Info&);
          virtual void unparseIfStmt(SgIfStmt*, SgUnparse_Info&);
          virtual void unparseImportStatement(SgImportStatement*, SgUnparse_Info&);
          virtual void unparseInitializedName(SgInitializedName*, SgUnparse_Info&);
          virtual void unparseKeyDatumList(SgKeyDatumList*, SgUnparse_Info&);
          virtual void unparseKeyDatumPair(SgKeyDatumPair*, SgUnparse_Info&);
          virtual void unparseLambdaRefExp(SgLambdaRefExp*, SgUnparse_Info&);
          virtual void unparseListExp(SgListExp*, SgUnparse_Info&);
          virtual void unparseListComprehension(SgListComprehension*, SgUnparse_Info&);
          virtual void unparseLongIntVal(SgLongIntVal*, SgUnparse_Info&);
          virtual void unparseNaryOp(SgNaryOp*, SgUnparse_Info&);
          virtual void unparsePassStatement(SgPassStatement*, SgUnparse_Info&);
          virtual void unparsePythonPrintStmt(SgPythonPrintStmt*, SgUnparse_Info&);
          virtual void unparseReturnStmt(SgReturnStmt*, SgUnparse_Info&);
          virtual void unparseSetComprehension(SgSetComprehension*, SgUnparse_Info&);
          virtual void unparseStmtDeclarationStatement(SgStmtDeclarationStatement*, SgUnparse_Info&);
          virtual void unparseStringVal(SgStringVal*, SgUnparse_Info&);
          virtual void unparseTryStmt(SgTryStmt*, SgUnparse_Info&);
          virtual void unparseTupleExp(SgTupleExp*, SgUnparse_Info&);
          virtual void unparseUnaryOp(SgUnaryOp*, SgUnparse_Info&);
          virtual void unparseVarRefExp(SgVarRefExp*, SgUnparse_Info&);
          virtual void unparseWhileStmt(SgWhileStmt*, SgUnparse_Info&);
          virtual void unparseYieldStatement(SgYieldStatement*, SgUnparse_Info&);

          virtual std::string ws_prefix(int nesting_level);
          virtual void unparseAsSuite(SgStatement* stmt, SgUnparse_Info&);
          virtual void unparseOperator(VariantT variant, bool pad = true);
   };

#define ROSE_PYTHON_AND_OP  "and"
#define ROSE_PYTHON_OR_OP   "or"
#define ROSE_PYTHON_NOT_OP  "not"

#endif /* UNPARSER_PYTHON_H_ */




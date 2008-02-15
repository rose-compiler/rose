/* unparser_fort.h
 *
 * This header file contains the class declaration for the Fortran
 * unparser.  The *fort*.C files implement this class.
 *
 */

#ifndef UNPARSER_FORT_H
#define UNPARSER_FORT_H

#include "rose.h"
#include "unparser.h"
#include "unparser_opt.h"
#include "unparse_format_fort.h"

#include "algorithm"


#ifndef UNPARSER_VERBOSE
//#define UNPARSER_VERBOSE TRUE
  #define UNPARSER_VERBOSE FALSE
#endif /* UNPARSER_VERBOSE */



//----------------------------------------------------------------------------
//! UnparserFort
//----------------------------------------------------------------------------

class UnparserFort : public UnparserBase
{
public:
  //! constructor
  UnparserFort() 
  : cur(NULL, NULL), repl(NULL)
  { }

  UnparserFort(ostream* localStream, char* filename, Unparser_Opt info, 
  	       int lineNumberToUnparse, UnparseFormatHelp* h = 0, 
	       UnparseDelegate* repl = 0);
  
  //! destructor
  virtual ~UnparserFort();
  
  //! begin the unparser
  //virtual void run_unparser();

  //! friend string globalUnparseToString ( SgNode* astNode );
  //virtual void unparseProject ( SgProject* project, SgUnparse_Info& info );
  virtual void unparseFile ( SgFile* file, SgUnparse_Info& info );

  //! Returns whether or not debugging information should be printed
  virtual bool isDebug () { return opt.get_debug_opt(); }
    
  //---------------------------------------------------------
  // utilities
  //---------------------------------------------------------

  char* getCurOutFileName();
  UnparseFormatFort& get_output_stream();

  //! Determines whether the given statement derived from the given file
  bool isStmtFromFile(SgStatement* stmt, char* sourceFilename);

  //---------------------------------------------------------
  //! unparse statement functions implememted in unparse_stmt_fort.C
  //---------------------------------------------------------
  void unparseStatement(SgStatement* stmt, SgUnparse_Info& info);
  
  void unparseGlobalStmt(SgStatement* stmt, SgUnparse_Info& info);
  void unparseScopeStmt(SgStatement* stmt, SgUnparse_Info& info);
  
  void unparseModuleStmt(SgStatement* stmt, SgUnparse_Info& info);
  void unparseProgHdrStmt(SgStatement* stmt, SgUnparse_Info& info);
  void unparseProcHdrStmt(SgStatement* stmt, SgUnparse_Info& info);
  
  void unparseInterfaceStmt(SgStatement* stmt, SgUnparse_Info& info);
  void unparseCommonBlock(SgStatement* stmt, SgUnparse_Info& info);
  void unparseVarDeclStmt(SgStatement* stmt, SgUnparse_Info& info);
  void unparseVarDefnStmt(SgStatement* stmt, SgUnparse_Info& info);
  void unparseParamDeclStmt(SgStatement* stmt, SgUnparse_Info& info);
  void unparseUseStmt(SgStatement* stmt, SgUnparse_Info& info);
  
  void unparseBasicBlockStmt(SgStatement* stmt, SgUnparse_Info& info);
  void unparseIfStmt(SgStatement* stmt, SgUnparse_Info& info);
  void unparseDoStmt(SgStatement* stmt, SgUnparse_Info& info);
  void unparseWhileStmt(SgStatement* stmt, SgUnparse_Info& info);
  void unparseSwitchStmt(SgStatement* stmt, SgUnparse_Info& info);
  void unparseCaseStmt(SgStatement* stmt, SgUnparse_Info& info);
  void unparseDefaultStmt(SgStatement* stmt, SgUnparse_Info& info);
  void unparseBreakStmt(SgStatement* stmt, SgUnparse_Info& info);
  void unparseContinueStmt(SgStatement* stmt, SgUnparse_Info& info);
  void unparseLabelStmt(SgStatement* stmt, SgUnparse_Info& info);
  void unparseGotoStmt(SgStatement* stmt, SgUnparse_Info& info);
  void unparseStopOrPauseStmt(SgStatement* stmt, SgUnparse_Info& info);
  void unparseReturnStmt(SgStatement* stmt, SgUnparse_Info& info);
  
  void unparseIOStmt(SgStatement* stmt, SgUnparse_Info& info);
  void unparseIOCtrlStmt(SgStatement* stmt, SgUnparse_Info& info);
  void unparseInOutStmt(SgStatement* stmt, SgUnparse_Info& info);

  void unparseExprStmt(SgStatement* stmt, SgUnparse_Info& info);
  
  void unparsePragmaDeclStmt(SgStatement* stmt, SgUnparse_Info& info);

  void unparseAttachedPreprocessingInfo(SgStatement* stmt, 
					SgUnparse_Info& info,
                                        PreprocessingInfo::RelativePositionType whereToUnparse);
  
  //! helpers
  void genPUAutomaticStmts(SgStatement* stmt, SgUnparse_Info& info);
  void unparseFuncArgs(SgInitializedNamePtrList* args, SgUnparse_Info& info);
  void unparseInitNamePtrList(SgInitializedNamePtrList* args, 
			      SgUnparse_Info& info);

  void unparseVarDecl(SgStatement* stmt, SgInitializedName* initializedName,
		      SgUnparse_Info& info);
  
  void printDeclModifier(SgDeclarationStatement* stmt, SgUnparse_Info& info);
  void printAccessModifier(SgDeclarationStatement* stmt, SgUnparse_Info& info);
  void printStorageModifier(SgDeclarationStatement* stmt, SgUnparse_Info& info);


  //---------------------------------------------------------
  //! unparse expression functions implemented in unparse_expr_fort.C
  //---------------------------------------------------------
  void unparseExpression(SgExpression* expr, SgUnparse_Info& info);

  void unparseExprRoot(SgExpression* expr, SgUnparse_Info& info);  

  void unparseFuncCall(SgExpression* expr, SgUnparse_Info& info);  
  void unparseIntrinsic(SgExpression* expr, SgUnparse_Info& info);
  
  void unparseUnaryExpr(SgExpression* expr, SgUnparse_Info& info);  
  void unparseBinaryExpr(SgExpression* expr, SgUnparse_Info& info); 
  void unparseUnaryOpr(SgExpression* expr, const char* op, 
		       SgUnparse_Info& info);
  void unparseBinaryOpr(SgExpression* expr, const char* op, 
			SgUnparse_Info& info);

  void unparseAssnOp(SgExpression* expr, SgUnparse_Info& info);  

  void unparseNotOp(SgExpression* expr, SgUnparse_Info& info);  
  void unparseAndOp(SgExpression* expr, SgUnparse_Info& info);  
  void unparseOrOp(SgExpression* expr, SgUnparse_Info& info);  

  void unparseEqOp(SgExpression* expr, SgUnparse_Info& info);  
  void unparseLtOp(SgExpression* expr, SgUnparse_Info& info);  
  void unparseGtOp(SgExpression* expr, SgUnparse_Info& info);  
  void unparseNeOp(SgExpression* expr, SgUnparse_Info& info);  
  void unparseLeOp(SgExpression* expr, SgUnparse_Info& info);  
  void unparseGeOp(SgExpression* expr, SgUnparse_Info& info);  

  void unparseUnaryMinusOp(SgExpression* expr, SgUnparse_Info& info);  
  void unparseUnaryAddOp(SgExpression* expr, SgUnparse_Info& info);  
  void unparseAddOp(SgExpression* expr, SgUnparse_Info& info);  
  void unparseSubtOp(SgExpression* expr, SgUnparse_Info& info);  
  void unparseMultOp(SgExpression* expr, SgUnparse_Info& info);  
  void unparseDivOp(SgExpression* expr, SgUnparse_Info& info);  
  void unparseIntDivOp(SgExpression* expr, SgUnparse_Info& info);  
  void unparseExpOp(SgExpression* expr, SgUnparse_Info& info);

  void unparseArrayOp(SgExpression* expr, SgUnparse_Info& info);  
  void unparseRecRef(SgExpression* expr, SgUnparse_Info& info);  
  void unparseCastOp(SgExpression* expr, SgUnparse_Info& info);  
  
  void unparseModOp(SgExpression* expr, SgUnparse_Info& info);  
  void unparseBitXOrOp(SgExpression* expr, SgUnparse_Info& info);  
  void unparseBitAndOp(SgExpression* expr, SgUnparse_Info& info);  
  void unparseBitOrOp(SgExpression* expr, SgUnparse_Info& info);  
  void unparseLShiftOp(SgExpression* expr, SgUnparse_Info& info);  
  void unparseRShiftOp(SgExpression* expr, SgUnparse_Info& info);  
  void unparseBitCompOp(SgExpression* expr, SgUnparse_Info& info);  

  void unparseNewOp(SgExpression* expr, SgUnparse_Info& info);  
  void unparseDeleteOp(SgExpression* expr, SgUnparse_Info& info);  

  void unparsePointStOp(SgExpression* expr, SgUnparse_Info& info);  
  void unparseDerefOp(SgExpression* expr, SgUnparse_Info& info);  
  void unparseAddrOp(SgExpression* expr, SgUnparse_Info& info);  
  void unparseTypeRef(SgExpression* expr, SgUnparse_Info& info);  
  void unparseSubExpr(SgExpression* expr, SgUnparse_Info& info);
  void unparseSubColon(SgExpression* expr, SgUnparse_Info& info);
  void unparseSubAsterick(SgExpression* expr, SgUnparse_Info& info); 
  
  void unparseExprInit(SgExpression* expr, SgUnparse_Info& info);  
  void unparseAggrInit(SgExpression* expr, SgUnparse_Info& info);  
  void unparseConInit(SgExpression* expr, SgUnparse_Info& info);
  void unparseAssnInit(SgExpression* expr, SgUnparse_Info& info);

  void unparseUseRename(SgExpression* expr, SgUnparse_Info& info);  
  void unparseUseOnly(SgExpression* expr, SgUnparse_Info& info);  

  void unparseIOItemExpr(SgExpression* expr, SgUnparse_Info& info);
  void unparseIOImpliedDo(SgExpression* expr, SgUnparse_Info& info);

  void unparseVarRef(SgExpression* expr, SgUnparse_Info& info);  
  void unparseFuncRef(SgExpression* expr, SgUnparse_Info& info);  
  void unparseMFuncRef(SgExpression* expr, SgUnparse_Info& info); 
  void unparseClassRef(SgExpression* expr, SgUnparse_Info& info);  
 
  void unparseBoolVal(SgExpression* expr, SgUnparse_Info& info);  
  void unparseCharVal(SgExpression* expr, SgUnparse_Info& info);  
  void unparseUCharVal(SgExpression* expr, SgUnparse_Info& info);  
  void unparseWCharVal(SgExpression* expr, SgUnparse_Info& info);  
  void unparseStringVal(SgExpression* expr, SgUnparse_Info& info);  
  void unparseShortVal(SgExpression* expr, SgUnparse_Info& info);  
  void unparseUShortVal(SgExpression* expr, SgUnparse_Info& info);  
  void unparseEnumVal(SgExpression* expr, SgUnparse_Info& info);  
  void unparseIntVal(SgExpression* expr, SgUnparse_Info& info);     
  void unparseUIntVal(SgExpression* expr, SgUnparse_Info& info);  
  void unparseLongIntVal(SgExpression* expr, SgUnparse_Info& info);  
  void unparseULongIntVal(SgExpression* expr, SgUnparse_Info& info); 
  void unparseLongLongIntVal(SgExpression* expr, SgUnparse_Info& info);    
  void unparseULongLongIntVal(SgExpression* expr, SgUnparse_Info& info);  
  void unparseFLoatVal(SgExpression* expr, SgUnparse_Info& info); 
  void unparseDblVal(SgExpression* expr, SgUnparse_Info& info);  
  void unparseLongDblVal(SgExpression* expr, SgUnparse_Info& info);  
  
  //! helpers
  bool unparseExprList(SgExprListExp* expr, SgUnparse_Info& info, 
		       bool paren = true);
  bool printStartParen(SgExpression* expr, SgUnparse_Info& info);
  bool isOperator(SgExpression* expr);
  bool isUnaryOperator(SgExpression* expr);
  bool isBinaryOperator(SgExpression* expr);
  bool isUnaryOperatorPlus(SgExpression* expr);
  bool isUnaryOperatorMinus(SgExpression* expr);
  
  
  //---------------------------------------------------------
  //! unparse symbol functions implemented in unparse_sym_fort.C
  //---------------------------------------------------------
  void unparseSymbol(SgSymbol* sym, SgUnparse_Info& info);

  void unparseVarSymbol(SgSymbol* sym, SgUnparse_Info& info);
  void unparseFunctionSymbol(SgSymbol* sym, SgUnparse_Info& info);
  void unparseMFunctionSymbol(SgSymbol* sym, SgUnparse_Info& info);
  void unparseIntrinsicSymbol(SgSymbol* sym, SgUnparse_Info& info);
  void unparseFuncTypeSymbol(SgSymbol* sym, SgUnparse_Info& info);
  void unparseClassSymbol(SgSymbol* sym, SgUnparse_Info& info);
  void unparseEnumSymbol(SgSymbol* sym, SgUnparse_Info& info);
  void unparseFieldSymbol(SgSymbol* sym, SgUnparse_Info& info);
  void unparseTypedefSymbol(SgSymbol* sym, SgUnparse_Info& info);
  void unparseLabelSymbol(SgSymbol* sym, SgUnparse_Info& info);


  //---------------------------------------------------------
  //! unparse type functions implemented in unparse_type_fort.C
  //---------------------------------------------------------
  void unparseType(SgType* type, SgUnparse_Info& info);
  void unparseTypeForConstExprs(SgType* type, SgUnparse_Info& info);
  void unparsePointerType(SgType* type, SgUnparse_Info& info);   
  void unparseMemberPointerType(SgType* type, SgUnparse_Info& info);    
  void unparseReferenceType(SgType* type, SgUnparse_Info& info);
  void unparseNameType(SgType* type, SgUnparse_Info& info);      
  void unparseClassType(SgType* type, SgUnparse_Info& info);     
  void unparseEnumType(SgType* type, SgUnparse_Info& info);      
  void unparseTypedefType(SgType* type, SgUnparse_Info& info);   
  void unparseModifierType(SgType* type, SgUnparse_Info& info);  
  void unparseFunctionType(SgType* type, SgUnparse_Info& info);  
  void unparseMemberFunctionType(SgType* type, SgUnparse_Info& info);
  void unparseArrayType(SgType* type, SgUnparse_Info& info);     
  
  bool isCharType(SgType* type);
  
private:
  //! holds all desired options for this unparser
  Unparser_Opt opt;

  //! Cursor mechanism (cur = cursor)
  UnparseFormatFort cur;

  //! delegate unparser that can be used to replace the output of this unparser
  UnparseDelegate *repl;

  // FIXME:eraxxon
  //! Supports unparsing include files
  char currentOutputFileName[256];
  int cur_index;
  int line_to_unparse;

};


#endif


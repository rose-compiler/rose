/* unparser.h
 * This header file contains the class declaration for the newest unparser. Six
 * C files include this header file: unparser.C, modified_sage.C, unparse_stmt.C, 
 * unparse_expr.C, unparse_type.C, and unparse_sym.C.
 */

#ifndef UNPARSER_H
#define UNPARSER_H

#include "rose.h"
#include "unparser_opt.h"
#include "unparse_format.h"

#include "algorithm"

// Macro used for debugging.  If TRUE it fixes the anonymous typedef and anonymous declaration
// bugs, but causes several other problems.  If FALSE, everything works except the anonymous 
// typedef and anonymous declaration bugs.
#define ANONYMOUS_TYPEDEF_FIX FALSE

// DQ (2/6/03):
// The unparser should not write to (modify) the AST.  This fix skips and locations
// in the unparser when the AST is modified.  It is an experimental fix.
// DQ (3/18/2004): This fix has works well for over a year now so I think we can 
// at some point remove code which was previously modifying the AST. Might
// want to check the locations of forward declarations of classes where they
// appear in the same file as their class definitions!
#define UNPARSER_IS_READ_ONLY

#define KAI_NONSTD_IOSTREAM 1

// DQ (3/18/2004): This is not removed as part of the newly added template support.
// #ifndef UNPARSE_TEMPLATES
// [DT] 
// #define UNPARSE_TEMPLATES TRUE
// #define UNPARSE_TEMPLATES FALSE
// #endif /* UNPARSE_TEMPLATES */

#ifndef UNPARSER_VERBOSE
// [DT] 8/14/2000 -- Toggle some of my debug output in the unparser.
//#define UNPARSER_VERBOSE TRUE
  #define UNPARSER_VERBOSE FALSE
#endif /* UNPARSER_VERBOSE */

// optionally turn off all directive processing (for debugging)
// define SKIP_UNPARSING_DIRECTIVES

// typedef map<int,int,less<int>,allocator<int> > X;
// typedef multimap<int,int,less<int>,allocator<int> > X;
// multimap<int,int,less<int>,allocator<int> > X;
// list<int> Y;
// typedef multimap<int,int,less<int> > X;

#if USE_OLD_MECHANISM_OF_HANDLING_PREPROCESSING_INFO
// I think this is part of the connection to lex support for comments
extern ROSEAttributesList* getPreprocessorDirectives( char *fileName);
#endif

 /* \if documentDevelopmentVersionUsingDoxygen
          \ingroup backendGenerator
          \brief This function is used by the SgNode object to connect the unparser to the AST.

          This function hides the complexity of generating a string from any subtree 
          of the AST (represented by a SgNode*).

          \internal This function uses the standard stringstream mechanism in C++ to 
                    convert the stream output to a string.
     \endif
  */
// string globalUnparseToString ( SgNode* astNode, SgUnparse_Info* inputUnparseInfoPointer = NULL );

void printOutComments ( SgLocatedNode* locatedNode );
string get_output_filename( SgFile& file);
//! returns the name of type t
string get_type_name( SgType* t);

//! Unparse the declaration as a string for use in prototypes within the AST rewrite mechanism prefix mechanism
string unparseDeclarationToString ( SgDeclarationStatement* declaration, bool unparseAsDeclaration = true );

//! Unparse the header of the scope statement (used in the prefix generation to 
//! permit context to be accounted for the in generation of AST fragements from 
//! strings, e.g. for cases in SgSwitchStatement).
string unparseScopeStatementWithoutBasicBlockToString ( SgScopeStatement* scope );

//! Unparse header of statements that have bodies (but are not scopes) (e.g. SgDefaultOptionStmt)
string unparseStatementWithoutBasicBlockToString      ( SgStatement* statement );


//-----------------------------------------------------------------------------------
//! UnparseDelegate
//-----------------------------------------------------------------------------------

class UnparseDelegate {
 public:
   virtual bool unparse_statement( SgStatement* stmt, SgUnparse_Info& info, UnparseFormatBase& out) = 0; 
};


//-----------------------------------------------------------------------------------
//! UnparserBase 
//-----------------------------------------------------------------------------------

// FIXME:eraxxon: --> To be renamed to Unparser
class UnparserBase
{
public:
  //! constructor
  UnparserBase() { }
  
  //! destructor
  virtual ~UnparserBase() { }
  
  //! begin the unparser
  //virtualvoid run_unparser() = 0;

  //! unparse 
  //virtual void unparseProject ( SgProject* project, SgUnparse_Info& info ) = 0;
  virtual void unparseFile ( SgFile* file, SgUnparse_Info& info ) = 0;
  
  //! Returns whether or not debugging information should be printed
  virtual bool isDebug () = 0;

  //---------------------------------------------------------
  // base utilities
  //---------------------------------------------------------

  //! get the filename from a Sage Statement Object
  char* getFileName(SgNode* stmt);
  
  //! TRUE if SgLocatedNode is part of a transformation on the AST
  bool isPartOfTransformation( SgLocatedNode *n);

  //! control printing of debugging information to standard output
  void printDebugInfo(const char*, bool);
  void printDebugInfo(int, bool);

private:
};


//-----------------------------------------------------------------------------------
//! Unparser
//-----------------------------------------------------------------------------------

// FIXME:eraxxon: --> To be renamed to UnparserCpp

class Unparser : public UnparserBase
{
public:
  //! constructor
  Unparser( ostream* localStream, char* filename, Unparser_Opt info, 
	    int lineNumberToUnparse, UnparseFormatHelp *h = 0, 
	    UnparseDelegate* repl = 0 );
  
  //! destructor
  virtual ~Unparser();

  //! begin the unparser (unparser.C)
  //virtual void run_unparser();
  
  //! friend string globalUnparseToString ( SgNode* astNode );
  //virtual void unparseProject ( SgProject* project, SgUnparse_Info& info );
  virtual void unparseFile ( SgFile* file, SgUnparse_Info& info );

  //! Returns whether or not debugging information should be printed
  virtual bool isDebug () { return opt.get_debug_opt(); }
  
  //---------------------------------------------------------
  // utilities
  //---------------------------------------------------------

  //! get the filename from the Sage File Object
  char* getCurOutFileName();

  //! get the output stream wrapper
  UnparseFormat& get_output_stream(); 

  //! Generate a CPP directive  
  void outputDirective ( PreprocessingInfo* directive );

  //! counts the number of lines in one directive
  int line_count(char*);
  
  //! counts the number of statements in a basic block
  int num_stmt_in_block(SgBasicBlock*);
  
  //! Used to decide which include files (most often header files) will be unparsed
  bool containsLanguageStatements (char* fileName);

  //! special case of extern "C" { \n\#include "foo.h" }
  bool includeFileIsSurroundedByExternCBraces ( char* tempFilename );

#if USE_OLD_MECHANISM_OF_HANDLING_PREPROCESSING_INFO
  //! functions that unparses directives and/or comments
  void unparseDirectives(SgStatement* stmt);
  void unparseFinalDirectives ( char* filename );
  
  void unparseDirectivesUptoButNotIncludingCurrentStatement ( SgStatement* stmt );
  void unparseDirectivesSharingLineWithStatement ( SgStatement* stmt );
  
  // lower level member function called by unparseDirectives() and unparseFinalDirectives()
  void unparseDirectives ( char* currentFilename, int currentPositionInListOfDirectives, int currentStatementLineNumber );
#else
  //! This is the new member function
  void unparseAttachedPreprocessingInfo(SgStatement* stmt, SgUnparse_Info& info,
                                        PreprocessingInfo::RelativePositionType whereToUnparse);
#endif
  
  
  //! functions that test for overloaded operator function (modified_sage.C)
  bool isOperator(SgExpression* expr);
  bool isBinaryEqualsOperator(SgExpression* expr);
  bool isBinaryEqualityOperator(SgExpression* expr);
  bool isBinaryInequalityOperator(SgExpression* expr);
  bool isBinaryArithmeticOperator(SgExpression* expr);
  bool isBinaryParenOperator(SgExpression* expr);
  bool isBinaryBracketOperator(SgExpression* expr);
  bool isBinaryOperator(SgExpression* expr);
  bool isUnaryOperatorPlus(SgExpression* expr);
  bool isUnaryOperatorMinus(SgExpression* expr);
  bool isUnaryOperator(SgExpression* expr);
  bool isUnaryPostfixOperator(SgExpression* expr);
  bool isOverloadedArrowOperator(SgExpression* expr);
  bool isIOStreamOperator(SgExpression* expr);

  bool isTransformed(SgStatement* stmt);
  
  //! auxiliary functions (some code from original modified_sage.C)
  bool NoDereference(SgExpression* expr);
  bool isCast_ConstCharStar(SgType* type);
  bool RemoveArgs(SgExpression* expr);
  bool PrintStartParen(SgExpression* expr, SgUnparse_Info& info);
  bool RemovePareninExprList(SgExprListExp* expr_list);
  bool isOneElementList(SgConstructorInitializer* con_init);
  void unparseOneElemConInit(SgConstructorInitializer* con_init, SgUnparse_Info& info);
  bool printConstructorName(SgExpression* expr);
  bool noQualifiedName(SgExpression* expr);
//  void output(SgLocatedNode* node);
  void directives(SgLocatedNode* lnode);

  void unparse_helper(SgFunctionDeclaration* funcdecl_stmt, SgUnparse_Info& info);

  void printSpecifier1(SgDeclarationStatement* decl_stmt, SgUnparse_Info& info);
  void printSpecifier2(SgDeclarationStatement* decl_stmt, SgUnparse_Info& info);
  void printSpecifier (SgDeclarationStatement* decl_stmt, SgUnparse_Info& info);

// DQ (4/3/2004): Added to output modifiers (e.g. register) in formal function arguments
  void printFunctionFormalArgumentSpecifier ( SgType* type, SgUnparse_Info& info );

// DQ (2/16/2004): Added to refactor code and add support for old-style K&R C
  void unparseFunctionArgs(SgFunctionDeclaration* funcdecl_stmt, SgUnparse_Info& info);
  void unparseFunctionParameterDeclaration ( SgFunctionDeclaration* funcdecl_stmt, 
                                             SgInitializedName* initializedName,
                                             bool outputParameterDeclaration, 
                                             SgUnparse_Info& info );

  //! remove unneccessary white space to build a condensed string
  static string removeUnwantedWhiteSpace ( const string & X );

  //! unparse symbol functions implemented in unparse_sym.C
  void unparseSymbol(SgSymbol* sym, SgUnparse_Info& info);
  void unparseVarSymbol(SgSymbol* sym, SgUnparse_Info& info);
  void unparseFunctionSymbol(SgSymbol* sym, SgUnparse_Info& info);
  void unparseFuncTypeSymbol(SgSymbol* sym, SgUnparse_Info& info);
  void unparseClassSymbol(SgSymbol* sym, SgUnparse_Info& info);
  void unparseUnionSymbol(SgSymbol* sym, SgUnparse_Info& info);
  void unparseStructSymbol(SgSymbol* sym, SgUnparse_Info& info);
  void unparseEnumSymbol(SgSymbol* sym, SgUnparse_Info& info);
  void unparseFieldSymbol(SgSymbol* sym, SgUnparse_Info& info);
  void unparseTypedefSymbol(SgSymbol* sym, SgUnparse_Info& info);
  void unparseMFunctionSymbol(SgSymbol* sym, SgUnparse_Info& info);
  void unparseLabelSymbol(SgSymbol* sym, SgUnparse_Info& info);
  void unparseConstructSymbol(SgSymbol* sym, SgUnparse_Info& info);
 
  //! unparse type functions implemented in unparse_type.C
  void unparseType(SgType* type, SgUnparse_Info& info);
  void unparsePointerType(SgType* type, SgUnparse_Info& info);   
  void unparseMemberPointerType(SgType* type, SgUnparse_Info& info);    
  void unparseReferenceType(SgType* type, SgUnparse_Info& info);
  void unparseNameType(SgType* type, SgUnparse_Info& info);      
  void unparseClassType(SgType* type, SgUnparse_Info& info);     
  void unparseStructType(SgType* type, SgUnparse_Info& info);    
  void unparseEnumType(SgType* type, SgUnparse_Info& info);      
  void unparseUnionType(SgType* type, SgUnparse_Info& info);     
  void unparseTypedefType(SgType* type, SgUnparse_Info& info);   
  void unparseModifierType(SgType* type, SgUnparse_Info& info);  
  void unparseFunctionType(SgType* type, SgUnparse_Info& info);  
  void unparseMemberFunctionType(SgType* type, SgUnparse_Info& info);
  void unparseArrayType(SgType* type, SgUnparse_Info& info);     

  //! unparse expression functions implemented in unparse_expr.C
  void unparseExpression(SgExpression* expr, SgUnparse_Info& info);
  void unparseUnaryOperator(SgExpression* expr, const char* op, SgUnparse_Info& info);
  void unparseBinaryOperator(SgExpression* expr, const char* op, SgUnparse_Info& info);
  void unparseUnaryExpr(SgExpression* expr, SgUnparse_Info& info);  
  void unparseBinaryExpr(SgExpression* expr, SgUnparse_Info& info); 
  void unparseAssnExpr(SgExpression* expr, SgUnparse_Info& info);   
  void unparseExprRoot(SgExpression* expr, SgUnparse_Info& info);  
  void unparseExprList(SgExpression* expr, SgUnparse_Info& info);  
  void unparseVarRef(SgExpression* expr, SgUnparse_Info& info);  
  void unparseClassRef(SgExpression* expr, SgUnparse_Info& info);  
  void unparseFuncRef(SgExpression* expr, SgUnparse_Info& info);  
  void unparseMFuncRef(SgExpression* expr, SgUnparse_Info& info);  
  void unparseBoolVal(SgExpression* expr, SgUnparse_Info& info);  
  void unparseShortVal(SgExpression* expr, SgUnparse_Info& info);  
  void unparseCharVal(SgExpression* expr, SgUnparse_Info& info);  
  void unparseUCharVal(SgExpression* expr, SgUnparse_Info& info);  
  void unparseWCharVal(SgExpression* expr, SgUnparse_Info& info);  
  void unparseStringVal(SgExpression* expr, SgUnparse_Info& info);  
  void unparseUShortVal(SgExpression* expr, SgUnparse_Info& info);  
  void unparseEnumVal(SgExpression* expr, SgUnparse_Info& info);  
  void unparseIntVal(SgExpression* expr, SgUnparse_Info& info);     
  void unparseUIntVal(SgExpression* expr, SgUnparse_Info& info);  
  void unparseLongIntVal(SgExpression* expr, SgUnparse_Info& info);  
  void unparseLongLongIntVal(SgExpression* expr, SgUnparse_Info& info);    
  void unparseULongLongIntVal(SgExpression* expr, SgUnparse_Info& info);  
  void unparseULongIntVal(SgExpression* expr, SgUnparse_Info& info); 
  void unparseFLoatVal                (SgExpression* expr, SgUnparse_Info& info); 
  void unparseDblVal                  (SgExpression* expr, SgUnparse_Info& info);  
  void unparseLongDblVal              (SgExpression* expr, SgUnparse_Info& info);  
  void unparseFuncCall                (SgExpression* expr, SgUnparse_Info& info);  
  void unparsePointStOp               (SgExpression* expr, SgUnparse_Info& info);  
  void unparseRecRef                  (SgExpression* expr, SgUnparse_Info& info);  
  void unparseDotStarOp               (SgExpression* expr, SgUnparse_Info& info);  
  void unparseArrowStarOp             (SgExpression* expr, SgUnparse_Info& info);  
  void unparseEqOp                    (SgExpression* expr, SgUnparse_Info& info);  
  void unparseLtOp                    (SgExpression* expr, SgUnparse_Info& info);  
  void unparseGtOp                    (SgExpression* expr, SgUnparse_Info& info);  
  void unparseNeOp                    (SgExpression* expr, SgUnparse_Info& info);  
  void unparseLeOp                    (SgExpression* expr, SgUnparse_Info& info);  
  void unparseGeOp                    (SgExpression* expr, SgUnparse_Info& info);  
  void unparseAddOp                   (SgExpression* expr, SgUnparse_Info& info);  
  void unparseSubtOp                  (SgExpression* expr, SgUnparse_Info& info);  
  void unparseMultOp                  (SgExpression* expr, SgUnparse_Info& info);  
  void unparseDivOp                   (SgExpression* expr, SgUnparse_Info& info);  
  void unparseIntDivOp                (SgExpression* expr, SgUnparse_Info& info);  
  void unparseModOp                   (SgExpression* expr, SgUnparse_Info& info);  
  void unparseAndOp                   (SgExpression* expr, SgUnparse_Info& info);  
  void unparseOrOp                    (SgExpression* expr, SgUnparse_Info& info);  
  void unparseBitXOrOp                (SgExpression* expr, SgUnparse_Info& info);  
  void unparseBitAndOp                (SgExpression* expr, SgUnparse_Info& info);  
  void unparseBitOrOp                 (SgExpression* expr, SgUnparse_Info& info);  
  void unparseCommaOp                 (SgExpression* expr, SgUnparse_Info& info);  
  void unparseLShiftOp                (SgExpression* expr, SgUnparse_Info& info);  
  void unparseRShiftOp                (SgExpression* expr, SgUnparse_Info& info);  
  void unparseUnaryMinusOp            (SgExpression* expr, SgUnparse_Info& info);  
  void unparseUnaryAddOp              (SgExpression* expr, SgUnparse_Info& info);  
  void unparseSizeOfOp                (SgExpression* expr, SgUnparse_Info& info);  
  void unparseTypeIdOp                (SgExpression* expr, SgUnparse_Info& info);  
  void unparseNotOp                   (SgExpression* expr, SgUnparse_Info& info);  
  void unparseDerefOp                 (SgExpression* expr, SgUnparse_Info& info);  
  void unparseAddrOp                  (SgExpression* expr, SgUnparse_Info& info);  
  void unparseMinusMinusOp            (SgExpression* expr, SgUnparse_Info& info);  
  void unparsePlusPlusOp              (SgExpression* expr, SgUnparse_Info& info);  
  void unparseAbstractOp              (SgExpression* expr, SgUnparse_Info& info);  
  void unparseBitCompOp               (SgExpression* expr, SgUnparse_Info& info);  
  void unparseExprCond                (SgExpression* expr, SgUnparse_Info& info);  
  void unparseClassInitOp             (SgExpression* expr, SgUnparse_Info& info);  
  void unparseDyCastOp                (SgExpression* expr, SgUnparse_Info& info);  
  void unparseCastOp                  (SgExpression* expr, SgUnparse_Info& info);  
  void unparseArrayOp                 (SgExpression* expr, SgUnparse_Info& info);  
  void unparseNewOp                   (SgExpression* expr, SgUnparse_Info& info);  
  void unparseDeleteOp                (SgExpression* expr, SgUnparse_Info& info);  
  void unparseThisNode                (SgExpression* expr, SgUnparse_Info& info);  
  void unparseScopeOp                 (SgExpression* expr, SgUnparse_Info& info);  
  void unparseAssnOp                  (SgExpression* expr, SgUnparse_Info& info);  
  void unparsePlusAssnOp              (SgExpression* expr, SgUnparse_Info& info);  
  void unparseMinusAssnOp             (SgExpression* expr, SgUnparse_Info& info);  
  void unparseAndAssnOp               (SgExpression* expr, SgUnparse_Info& info);  
  void unparseIOrAssnOp               (SgExpression* expr, SgUnparse_Info& info);  
  void unparseMultAssnOp              (SgExpression* expr, SgUnparse_Info& info);  
  void unparseDivAssnOp               (SgExpression* expr, SgUnparse_Info& info);  
  void unparseModAssnOp               (SgExpression* expr, SgUnparse_Info& info);  
  void unparseXorAssnOp               (SgExpression* expr, SgUnparse_Info& info);  
  void unparseLShiftAssnOp            (SgExpression* expr, SgUnparse_Info& info);  
  void unparseRShiftAssnOp            (SgExpression* expr, SgUnparse_Info& info);  
  void unparseForDeclOp               (SgExpression* expr, SgUnparse_Info& info);  
  void unparseTypeRef                 (SgExpression* expr, SgUnparse_Info& info);  
  void unparseVConst                  (SgExpression* expr, SgUnparse_Info& info);  
  void unparseExprInit                (SgExpression* expr, SgUnparse_Info& info);  
  void unparseAggrInit                (SgExpression* expr, SgUnparse_Info& info);  
  void unparseConInit                 (SgExpression* expr, SgUnparse_Info& info);
  void unparseAssnInit                (SgExpression* expr, SgUnparse_Info& info);
  void unparseThrowOp                 (SgExpression* expr, SgUnparse_Info& info);
  void unparseVarArgStartOp           (SgExpression* expr, SgUnparse_Info& info);
  void unparseVarArgStartOneOperandOp (SgExpression* expr, SgUnparse_Info& info);
  void unparseVarArgOp                (SgExpression* expr, SgUnparse_Info& info);
  void unparseVarArgEndOp             (SgExpression* expr, SgUnparse_Info& info);
  void unparseVarArgCopyOp            (SgExpression* expr, SgUnparse_Info& info);

  //! unparse statement functions implememted in unparse_stmt.C
  void unparseStatement        (SgStatement* stmt, SgUnparse_Info& info);
  void unparseGlobalStmt       (SgStatement* stmt, SgUnparse_Info& info);
  void unparseDeclStmt         (SgStatement* stmt, SgUnparse_Info& info);
  void unparseScopeStmt        (SgStatement* stmt, SgUnparse_Info& info);
  void unparseFuncTblStmt      (SgStatement* stmt, SgUnparse_Info& info);
  void unparseBasicBlockStmt   (SgStatement* stmt, SgUnparse_Info& info);
  void unparseElseIfStmt       (SgStatement* stmt, SgUnparse_Info& info);
  void unparseIfStmt           (SgStatement* stmt, SgUnparse_Info& info);
  void unparseWhereStmt        (SgStatement* stmt, SgUnparse_Info& info);
  void unparseForInitStmt      (SgStatement* stmt, SgUnparse_Info& info);
  void unparseForStmt          (SgStatement* stmt, SgUnparse_Info& info);
  void unparseFuncDeclStmt     (SgStatement* stmt, SgUnparse_Info& info);
  void unparseFuncDefnStmt     (SgStatement* stmt, SgUnparse_Info& info);
  void unparseMFuncDeclStmt    (SgStatement* stmt, SgUnparse_Info& info);
  void unparseVarDeclStmt      (SgStatement* stmt, SgUnparse_Info& info);
  void unparseVarDefnStmt      (SgStatement* stmt, SgUnparse_Info& info);
  void unparseClassDeclStmt    (SgStatement* stmt, SgUnparse_Info& info);
  void unparseClassDefnStmt    (SgStatement* stmt, SgUnparse_Info& info);
  void unparseEnumDeclStmt     (SgStatement* stmt, SgUnparse_Info& info);
  void unparseExprStmt         (SgStatement* stmt, SgUnparse_Info& info);
  void unparseLabelStmt        (SgStatement* stmt, SgUnparse_Info& info);
  void unparseWhileStmt        (SgStatement* stmt, SgUnparse_Info& info);
  void unparseDoWhileStmt      (SgStatement* stmt, SgUnparse_Info& info);
  void unparseSwitchStmt       (SgStatement* stmt, SgUnparse_Info& info);
  void unparseCaseStmt         (SgStatement* stmt, SgUnparse_Info& info);
  void unparseTryStmt          (SgStatement* stmt, SgUnparse_Info& info);
  void unparseCatchStmt        (SgStatement* stmt, SgUnparse_Info& info);
  void unparseDefaultStmt      (SgStatement* stmt, SgUnparse_Info& info);
  void unparseBreakStmt        (SgStatement* stmt, SgUnparse_Info& info);
  void unparseContinueStmt     (SgStatement* stmt, SgUnparse_Info& info);
  void unparseReturnStmt       (SgStatement* stmt, SgUnparse_Info& info);
  void unparseGotoStmt         (SgStatement* stmt, SgUnparse_Info& info);
  void unparseAsmStmt          (SgStatement* stmt, SgUnparse_Info& info);
  void unparseSpawnStmt        (SgStatement* stmt, SgUnparse_Info& info);
  void unparseParStmt          (SgStatement* stmt, SgUnparse_Info& info);
  void unparseParForStmt       (SgStatement* stmt, SgUnparse_Info& info);
  void unparseTypeDefStmt      (SgStatement* stmt, SgUnparse_Info& info);
  void unparseTemplateDeclStmt (SgStatement* stmt, SgUnparse_Info& info);

// DQ (2/29/2004): Added to support unparsing of template instantiations (similar to class declarations)
  void unparseTemplateInstantiationDeclStmt               (SgStatement* stmt, SgUnparse_Info& info);

// DQ (3/24/2004): Added to support template functions and template member functions
  void unparseTemplateInstantiationFunctionDeclStmt       (SgStatement* stmt, SgUnparse_Info& info);
  void unparseTemplateInstantiationMemberFunctionDeclStmt (SgStatement* stmt, SgUnparse_Info& info);

  void unparsePragmaDeclStmt   (SgStatement* stmt, SgUnparse_Info& info);

// DQ (3/13/2004): Added to support templates
  void unparseTemplateParameter(SgTemplateParameter* templateParameter, SgUnparse_Info& info);
  void unparseTemplateArgument (SgTemplateArgument*  templateArgument , SgUnparse_Info& info);

// DQ (5/1/2004): Added support for unparsing namespace constructs
  void unparseNamespaceDeclarationStatement      ( SgStatement* stmt, SgUnparse_Info & info );
  void unparseNamespaceDefinitionStatement       ( SgStatement* stmt, SgUnparse_Info & info );
  void unparseNamespaceAliasDeclarationStatement ( SgStatement* stmt, SgUnparse_Info & info );
  void unparseUsingDirectiveStatement            ( SgStatement* stmt, SgUnparse_Info & info );
  void unparseUsingDeclarationStatement          ( SgStatement* stmt, SgUnparse_Info & info );

// DQ (10/14/2004): Supporting function shared by unparseClassDecl and unparseClassType
  void initializeDeclarationsFromParent ( SgDeclarationStatement* declarationStatement,
                                          SgClassDefinition* & cdefn,
                                          SgNamespaceDefinitionStatement* & namespaceDefn );

  //! used to support the run_unparser function
  //! (support for #line 42 "filename" when it appears in source code)
  bool statementFromFile ( SgStatement* stmt, char* sourceFilename );

#if USE_OLD_MECHANISM_OF_HANDLING_PREPROCESSING_INFO
  void getDirectives ( char* sourceFilename );
#endif

  //! incomplete-documentation
  bool isASecondaryFile ( SgStatement* stmt );

private:

  //! Used to support unparsing of doubles and long double as x.0 instead of just x if they are whole number values.
  bool zeroRemainder( long double doubleValue );

  //! default constructor
  //Unparser() {};

  //! Supports unparsing include files
  //char primaryOutputFileName[256];
  //! Supports unparsing include files
  char currentOutputFileName[256];

  //! holds all desired options for this unparser
  Unparser_Opt opt;
  //! used to index the preprocessor list
  int cur_index;
  //! The previous directive was a CPP statment (otherwise it was a comment)
  bool prevdir_was_cppDeclaration;
  //! the line number of the statement/directive to be unparsed note that 0 means unparse ALL lines
  int line_to_unparse;

  //! Somesort of cursor mechanism added by Qing Yi
  UnparseFormat cur;

  //! delegate unparser that can be used to replace the output of this unparser
  UnparseDelegate *repl;

#if USE_OLD_MECHANISM_OF_HANDLING_PREPROCESSING_INFO
/*! \brief Old approach to handling comments separately from the AST, it 
           is included for compatability while it is evaluated.
    \depricated This approach is currently not used (will be removed soon).
 */
   ROSEAttributesListContainer directiveListContainer;
#endif

};

void unparseFile ( SgFile & file, UnparseDelegate *repl  = 0 );
#endif







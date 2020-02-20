#ifndef UNPARSER_JOVIAL
#define UNPARSER_JOVIAL

#include "unparser.h"

#include "Cxx_Grammar.h"
#include "unparseLanguageIndependentConstructs.h"


class Unparse_Jovial : public UnparseLanguageIndependentConstructs
   {
     public:
          Unparse_Jovial(Unparser* unp, std::string fname);

          virtual ~Unparse_Jovial();

          virtual std::string languageName() const { return "Jovial Unparser"; }

          void unparseJovialFile(SgSourceFile *file, SgUnparse_Info &info);

          virtual void unparseLanguageSpecificStatement  (SgStatement* stmt,  SgUnparse_Info& info);
          virtual void unparseLanguageSpecificExpression (SgExpression* expr, SgUnparse_Info& info);

          virtual void unparseStringVal              (SgExpression* expr, SgUnparse_Info& info);  

          virtual void unparseDefineDeclStmt         (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseDirectiveStmt          (SgStatement* stmt, SgUnparse_Info& info);

          virtual void unparseCompoolStmt            (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseProgHdrStmt            (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseFuncDeclStmt           (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseFuncDefnStmt           (SgStatement* stmt, SgUnparse_Info& info);

          virtual void unparseBasicBlockStmt         (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseLabelStmt              (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseForStatement           (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseJovialForThenStatement (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseWhileStmt              (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseGotoStmt               (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseIfStmt                 (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseSwitchStmt             (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseCaseStmt               (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseDefaultStmt            (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseBreakStmt              (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseTypeDefStmt            (SgStatement* stmt, SgUnparse_Info& info);


          virtual void unparseStopOrPauseStmt        (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseReturnStmt             (SgStatement* stmt, SgUnparse_Info& info);

          virtual void unparseEnumDeclStmt           (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseTableDeclStmt          (SgStatement* stmt, SgUnparse_Info& info);

          virtual void unparseVarDeclStmt            (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseVarDecl                (SgStatement* stmt, SgInitializedName* initializedName, SgUnparse_Info& info);

          virtual void unparseExprStmt               (SgStatement* stmt, SgUnparse_Info& info);

       // Types
          virtual void unparseType        (SgType* type,  SgUnparse_Info& info);
          virtual void unparseTypeFixed   (SgType* type,  SgUnparse_Info& info);
          virtual void unparseTypeSize    (SgType* expr,  SgUnparse_Info& info);
          virtual void unparseArrayType   (SgType* type,  SgUnparse_Info& info);
          virtual void unparseTableType   (SgType* type,  SgUnparse_Info& info);

#if 0
          virtual void unparseTypeVoid(SgTypeVoid* type, SgUnparse_Info& info);

          virtual void unparseBaseClass(SgBaseClass* base, SgUnparse_Info& info);
          virtual void unparseParameterType(SgType *bound_type, SgUnparse_Info& info);

          virtual void unparseTypeWchar(SgTypeWchar* type, SgUnparse_Info& info);
          virtual void unparseTypeSignedChar(SgTypeSignedChar* type, SgUnparse_Info& info);
          virtual void unparseTypeShort(SgTypeShort* type, SgUnparse_Info& info);
          virtual void unparseTypeInt(SgTypeInt* type, SgUnparse_Info& info);
#endif

       // Expressions
          virtual void unparseUnaryOperator  (SgExpression* expr, const char* op, SgUnparse_Info& info);
          virtual void unparseBinaryOperator (SgExpression* expr, const char* op, SgUnparse_Info& info);

          virtual void unparseSubscriptExpr     (SgExpression* expr, SgUnparse_Info& info);
          virtual void unparseArrayOp           (SgExpression* expr, SgUnparse_Info& info);
          virtual void unparseAsteriskShapeExpr (SgExpression* expr, SgUnparse_Info& info);

       // virtual void unparseExpression     (SgExpression* expr, SgUnparse_Info& info);
       // virtual void unparseUnaryExpr      (SgExpression* expr, SgUnparse_Info& info);
       // virtual void unparseBinaryExpr     (SgExpression* expr, SgUnparse_Info& info);

          virtual void unparseAssignOp       (SgExpression* expr, SgUnparse_Info& info);   
          virtual void unparseFuncRef        (SgExpression* expr, SgUnparse_Info& info);
          virtual void unparseVarRef         (SgExpression* expr, SgUnparse_Info& info);

          virtual void unparseFuncCall       (SgExpression* expr, SgUnparse_Info& info);

       // Initializers
          virtual void unparseAssnInit       (SgExpression* expr, SgUnparse_Info& info);

       // Table dimension list
          void unparseDimInfo (SgExprListExp* dim_info, SgUnparse_Info& info);

#if 0
          void unparseOneElemConInit(SgConstructorInitializer* con_init, SgUnparse_Info& info);

      //! Unparser support for compiler-generated statments
          void outputCompilerGeneratedStatements( SgUnparse_Info & info );

       // DQ (8/14/2007): This is where all the langauge specific statement unparing is done
          virtual void unparseLanguageSpecificStatement (SgStatement*  stmt, SgUnparse_Info& info);
          virtual void unparseLanguageSpecificExpression(SgExpression* expr, SgUnparse_Info& info);

          virtual void unparseFunctionParameterList(SgStatement* stmt, SgUnparse_Info& info);

       // DQ (9/6/2010): Mark the derived class to support debugging.
          virtual std::string languageName() const { return "Jovial Unparser"; }

          virtual void unparseVarRef                  (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseClassRef                (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseMFuncRef                (SgExpression* expr, SgUnparse_Info& info);

       // DQ (11/10/2005): Added general support for SgValue (so that we could unparse 
       // expression trees from contant folding)
          virtual void unparseStringVal               (SgExpression* expr, SgUnparse_Info& info);  

       // these need Jovial-specific treatment
          virtual void unparseBoolVal                 (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseWCharVal                (SgExpression* expr, SgUnparse_Info& info);  

          virtual void unparseUIntVal                 (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseLongIntVal              (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseLongLongIntVal          (SgExpression* expr, SgUnparse_Info& info);    
          virtual void unparseULongLongIntVal         (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseULongIntVal             (SgExpression* expr, SgUnparse_Info& info); 
          virtual void unparseFloatVal                (SgExpression* expr, SgUnparse_Info& info); 
          virtual void unparseLongDoubleVal           (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseComplexVal              (SgExpression* expr, SgUnparse_Info& info);

          virtual void unparseUnaryMinusOp            (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseUnaryAddOp              (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseSizeOfOp                (SgExpression* expr, SgUnparse_Info& info);  

          virtual void unparseTypeIdOp                (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseNotOp                   (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseAbstractOp              (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseBitCompOp               (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseExprCond                (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseClassInitOp             (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseDyCastOp                (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseCastOp                  (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseArrayOp                 (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseNewOp                   (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseDeleteOp                (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseThisNode                (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseSuperNode               (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseClassNode               (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseScopeOp                 (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseForDeclOp               (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseTypeRef                 (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseVConst                  (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseExprInit                (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseAggrInit                (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseConInit                 (SgExpression* expr, SgUnparse_Info& info);
          virtual void unparseAssnInit                (SgExpression* expr, SgUnparse_Info& info);
          virtual void unparseThrowOp                 (SgExpression* expr, SgUnparse_Info& info);
       // virtual void unparseVarArgStartOp           (SgExpression* expr, SgUnparse_Info& info);
       // virtual void unparseVarArgStartOneOperandOp (SgExpression* expr, SgUnparse_Info& info);
       // virtual void unparseVarArgOp                (SgExpression* expr, SgUnparse_Info& info);
       // virtual void unparseVarArgEndOp             (SgExpression* expr, SgUnparse_Info& info);
       // virtual void unparseVarArgCopyOp            (SgExpression* expr, SgUnparse_Info& info);
          virtual void unparseDesignatedInitializer   (SgExpression* expr, SgUnparse_Info& info);
          virtual void unparsePseudoDtorRef           (SgExpression* expr, SgUnparse_Info& info);
          virtual void unparseX10InstanceOfOp        (SgExpression* expr, SgUnparse_Info& info);

          virtual void unparseCaseOrDefaultBasicBlockStmt   (SgStatement* stmt, SgUnparse_Info& info);

          virtual void unparseIfStmt           (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseSynchronizedStmt (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseThrowStmt        (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseForEachStmt      (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseForInitStmt      (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseForStmt          (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseFuncDeclStmt     (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseFuncDefnStmt     (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseMFuncDeclStmt    (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseVarDeclStmt      (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseVarDefnStmt      (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseX10PackageStmt  (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseClassDeclStmt    (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseClassDefnStmt    (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseLabelStmt        (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseWhileStmt        (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseDoWhileStmt      (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseSwitchStmt       (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseCaseStmt         (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseTryStmt          (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseCatchStmt        (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseDefaultStmt      (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseBreakStmt        (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseContinueStmt     (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseAssertStmt       (SgStatement* stmt, SgUnparse_Info& info);


      //! DQ (10/12/2006): Support for qualified names (function names can't have global scope specifier in GNU, or so it seems).
          std::string trimGlobalScopeQualifier ( std::string qualifiedName );

      //! Unparse intialized names (technically not an SgStatement, but both share SgLocatedNode)
          virtual void unparseInitializedName(SgInitializedName* init_name, SgUnparse_Info& info);
          virtual void unparseName(SgName name, SgUnparse_Info& info);

          virtual void unparseCompoundAssignOp(SgCompoundAssignOp* op, SgUnparse_Info& info);
          virtual void unparseBinaryOp(SgBinaryOp* op, SgUnparse_Info& info);
          virtual void unparseUnaryOp(SgUnaryOp* op, SgUnparse_Info& info);

      //! Support for unparsing modifiers
          virtual void unparseDeclarationModifier   (SgDeclarationModifier& mod,   SgUnparse_Info& info);
          virtual void unparseAccessModifier        (SgAccessModifier& mod,        SgUnparse_Info& info);
          virtual void unparseStorageModifier       (SgStorageModifier& mod,       SgUnparse_Info& info);
          virtual void unparseConstVolatileModifier (SgConstVolatileModifier& mod, SgUnparse_Info& info);
          virtual void unparseTypeModifier          (SgTypeModifier& mod,          SgUnparse_Info& info);
          virtual void unparseFunctionModifier      (SgFunctionModifier& mod,      SgUnparse_Info& info);

          virtual void curprint_indented(const std::string str, SgUnparse_Info& info) const;

          virtual void unparseNestedStatement(SgStatement* stmt, SgUnparse_Info& info);

          virtual PrecedenceSpecifier getPrecedence(SgExpression* exp);
          virtual AssociativitySpecifier getAssociativity(SgExpression* exp);

      //! Support for unparsing types
          virtual void unparseTypeVoid(SgTypeVoid* type, SgUnparse_Info& info);

          virtual void unparseBaseClass(SgBaseClass* base, SgUnparse_Info& info);
          virtual void unparseParameterType(SgType *bound_type, SgUnparse_Info& info);

          virtual void unparseTypeWchar(SgTypeWchar* type, SgUnparse_Info& info);
          virtual void unparseTypeSignedChar(SgTypeSignedChar* type, SgUnparse_Info& info);
          virtual void unparseTypeShort(SgTypeShort* type, SgUnparse_Info& info);
          virtual void unparseTypeInt(SgTypeInt* type, SgUnparse_Info& info);
          virtual void unparseTypeLong(SgTypeLong* type, SgUnparse_Info& info);
          virtual void unparseTypeFloat(SgTypeFloat* type, SgUnparse_Info& info);
          virtual void unparseTypeDouble(SgTypeDouble* type, SgUnparse_Info& info);
          virtual void unparseTypeBool(SgTypeBool* type, SgUnparse_Info& info);

          virtual void unparseTypedefType(SgTypedefType* type, SgUnparse_Info& info);
          virtual void unparseClassType(SgClassType* type, SgUnparse_Info& info);
          virtual void unparseEnumType(SgEnumType* type, SgUnparse_Info& info);
          virtual void unparseArrayType(SgArrayType* type, SgUnparse_Info& info);
          virtual void unparseModifierType(SgModifierType* type, SgUnparse_Info& info);

          virtual bool requiresParentheses(SgExpression* expr, SgUnparse_Info& info);
          virtual void unparseExpression(SgExpression* expr, SgUnparse_Info& info);
          virtual void unparseUnaryExpr(SgExpression* expr, SgUnparse_Info& info);
          virtual void unparseBinaryExpr(SgExpression* expr, SgUnparse_Info& info);

          void unparseEnumBody(SgClassDefinition *, SgUnparse_Info& info);
#endif
   };

#endif

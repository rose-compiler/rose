#ifndef UNPARSER_ADA
#define UNPARSER_ADA

#include "unparser.h"

#if 1
class SgExpression;
class SgStatement;
class SgTemplateParameter;
class SgTemplateArgument;
class SgFunctionDeclaration;
class SgBasicBlock;
class SgClassDefinition;
class SgTemplateInstantiationDecl;
class SgNamespaceDefinitionStatement;
class SgAsmOp;
#endif

#include "Cxx_Grammar.h"
#include "unparseLanguageIndependentConstructs.h"

class Unparser;

struct Unparse_Ada : UnparseLanguageIndependentConstructs
   {
          typedef UnparseLanguageIndependentConstructs base;
          
          Unparse_Ada(Unparser* unp, std::string fname);

          void unparseAdaFile(SgSourceFile *file, SgUnparse_Info &info);

          void unparseStatement(SgStatement* stmt, SgUnparse_Info& info) ROSE_OVERRIDE;
          void unparseLanguageSpecificStatement  (SgStatement* stmt,  SgUnparse_Info& info) ROSE_OVERRIDE;
          void unparseLanguageSpecificExpression (SgExpression* expr, SgUnparse_Info& info) ROSE_OVERRIDE;
          
          bool requiresParentheses(SgExpression* expr, SgUnparse_Info& info) ROSE_OVERRIDE;

          // DQ (9/12/2017): Mark the derived class to support debugging.
          // virtual std::string languageName() const;
          std::string languageName() const ROSE_OVERRIDE { return "Ada Unparser"; }

          void unparseStringVal(SgExpression* expr, SgUnparse_Info& info) ROSE_OVERRIDE;  
          //~ void unparseStatement(SgStatement* stmt, SgUnparse_Info& info) ROSE_OVERRIDE;
          
          void unparseType(SgType* type, SgUnparse_Info& info);

#if 0
       // DQ (3/13/2004): Added to support templates
          virtual void unparseTemplateParameter(SgTemplateParameter* templateParameter, SgUnparse_Info& info);
          virtual void unparseTemplateArgument (SgTemplateArgument*  templateArgument , SgUnparse_Info& info);

       // DQ (11/27/2004): Added to support unparsing of pointers to nested template arguments 
          virtual void unparseTemplateName(SgTemplateInstantiationDecl* templateInstantiationDeclaration, SgUnparse_Info& info);


          void unparseOneElemConInit(SgConstructorInitializer* con_init, SgUnparse_Info& info);

      //! Unparser support for compiler-generated statments
          void outputCompilerGeneratedStatements( SgUnparse_Info & info );

       // DQ (8/14/2007): This is where all the langauge specific statement unparing is done
          virtual void unparseLanguageSpecificStatement (SgStatement*  stmt, SgUnparse_Info& info);
          virtual void unparseLanguageSpecificExpression(SgExpression* expr, SgUnparse_Info& info);

          virtual void unparseFunctionParameterList(SgStatement* stmt, SgUnparse_Info& info);

       // DQ (9/6/2010): Mark the derived class to support debugging.
          virtual std::string languageName() const { return "Ada Unparser"; }

          virtual void unparseUnaryOperator           (SgExpression* expr, const char* op, SgUnparse_Info& info);
          virtual void unparseAssnExpr                (SgExpression* expr, SgUnparse_Info& info);   
          virtual void unparseVarRef                  (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseClassRef                (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseFuncRef                 (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseMFuncRef                (SgExpression* expr, SgUnparse_Info& info);

       // DQ (11/10/2005): Added general support for SgValue (so that we could unparse 
       // expression trees from contant folding)
          virtual void unparseStringVal               (SgExpression* expr, SgUnparse_Info& info);  

       // these need Ada-specific treatment
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


          virtual void unparseFuncCall                (SgExpression* expr, SgUnparse_Info& info);  
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

//          virtual void unparseGlobalStmt       (SgStatement* stmt, SgUnparse_Info& info);

          virtual void unparseBasicBlockStmt   (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseCaseOrDefaultBasicBlockStmt   (SgStatement* stmt, SgUnparse_Info& info);

       // DQ (4/16/2011): Added X10 specific "import" statement.
          virtual void unparseImportDeclarationStatement (SgStatement* stmt, SgUnparse_Info& info);

          virtual void unparseIfStmt           (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseSynchronizedStmt (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseThrowStmt        (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseForEachStmt      (SgStatement* stmt, SgUnparse_Info& info);
       // virtual void unparseWhereStmt        (SgStatement* stmt, SgUnparse_Info& info);
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
          virtual void unparseEnumDeclStmt     (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseExprStmt         (SgStatement* stmt, SgUnparse_Info& info);
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
          virtual void unparseReturnStmt       (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseAssertStmt       (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseGotoStmt         (SgStatement* stmt, SgUnparse_Info& info);
       // virtual void unparseAsmStmt          (SgStatement* stmt, SgUnparse_Info& info);
#if 0
          virtual void unparseFinishStmt        (SgFinishStmt* stmt, SgUnparse_Info& info);
          virtual void unparseAtStmt                    (SgAtStmt* stmt, SgUnparse_Info& info);
          virtual void unparseAsyncStmt                 (SgAsyncStmt* stmt, SgUnparse_Info& info);
#endif
          virtual void unparseTypeDefStmt      (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseTemplateDeclStmt (SgStatement* stmt, SgUnparse_Info& info);

       // DQ (7/21/2006): Added support for GNU statement expression extension.
       // virtual void unparseStatementExpression (SgExpression* expr, SgUnparse_Info& info);

      // DQ (7/22/2006): Added support for asm operands.
      // virtual void unparseAsmOp (SgExpression* expr, SgUnparse_Info& info);
      // virtual void unparse_asm_operand_modifier(SgAsmOp::asm_operand_modifier_enum flags);
      // std::string unparse_register_name (SgInitializedName::asm_register_name_enum register_name);

       // DQ (2/29/2004): Added to support unparsing of template instantiations (similar to class declarations)
          virtual void unparseTemplateInstantiationDeclStmt               (SgStatement* stmt, SgUnparse_Info& info);

       // DQ (3/24/2004): Added to support template functions and template member functions
          virtual void unparseTemplateInstantiationFunctionDeclStmt       (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseTemplateInstantiationMemberFunctionDeclStmt (SgStatement* stmt, SgUnparse_Info& info);

       // DQ (4/16/2005): Added support for explicit template directives
          virtual void unparseTemplateInstantiationDirectiveStmt          (SgStatement* stmt, SgUnparse_Info& info);

       // virtual void unparsePragmaDeclStmt   (SgStatement* stmt, SgUnparse_Info& info);

       // DQ (5/1/2004): Added support for unparsing namespace constructs
          virtual void unparseNamespaceDeclarationStatement      ( SgStatement* stmt, SgUnparse_Info & info );
          virtual void unparseNamespaceDefinitionStatement       ( SgStatement* stmt, SgUnparse_Info & info );
          virtual void unparseNamespaceAliasDeclarationStatement ( SgStatement* stmt, SgUnparse_Info & info );
          virtual void unparseUsingDirectiveStatement            ( SgStatement* stmt, SgUnparse_Info & info );
          virtual void unparseUsingDeclarationStatement          ( SgStatement* stmt, SgUnparse_Info & info );

       // DQ (10/14/2004): Supporting function shared by unparseClassDecl and unparseClassType
          void initializeDeclarationsFromParent ( SgDeclarationStatement* declarationStatement,
                                                  SgClassDefinition* & cdefn,
                                                  SgNamespaceDefinitionStatement* & namespaceDefn, int debugSupport = 0 );


      //! DQ (10/12/2006): Support for qualified names (function names can't have global scope specifier in GNU, or so it seems).
          std::string trimGlobalScopeQualifier ( std::string qualifiedName );

      //! Support for exception specification for functions and member functions
          virtual void unparseExceptionSpecification( const SgTypePtrList& exceptionSpecifierList, SgUnparse_Info& info);

      //! Unparse intialized names (technically not an SgStatement, but both share SgLocatedNode)
          virtual void unparseInitializedName(SgInitializedName* init_name, SgUnparse_Info& info);
          virtual void unparseName(SgName name, SgUnparse_Info& info);

          virtual void unparseCompoundAssignOp(SgCompoundAssignOp* op, SgUnparse_Info& info);
          virtual void unparseBinaryOp(SgBinaryOp* op, SgUnparse_Info& info);
          virtual void unparseUnaryOp(SgUnaryOp* op, SgUnparse_Info& info);

      //! Support for unparsing modifiers
          virtual void unparseDeclarationModifier   (SgDeclarationModifier& mod,   SgUnparse_Info& info);
//          virtual void unparseDeclarationModifier2   (SgDeclarationModifier& mod,   SgUnparse_Info& info);
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
          virtual void unparseType(SgType* type, SgUnparse_Info& info);
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

          virtual void unparseX10QualifiedType(SgJavaQualifiedType *type, SgUnparse_Info& info);
          virtual void unparseX10ParameterizedType(SgJavaParameterizedType* type, SgUnparse_Info& info);
          virtual void unparseX10WildcardType(SgJavaWildcardType* type, SgUnparse_Info& info);
          virtual void unparseX10UnionType(SgJavaUnionType* type, SgUnparse_Info& info);

          virtual bool requiresParentheses(SgExpression* expr, SgUnparse_Info& info);
          virtual void unparseExpression(SgExpression* expr, SgUnparse_Info& info);
          virtual void unparseUnaryExpr(SgExpression* expr, SgUnparse_Info& info);
          virtual void unparseBinaryExpr(SgExpression* expr, SgUnparse_Info& info);


          void unparseEnumBody(SgClassDefinition *, SgUnparse_Info& info);
          void unparseTypeParameters(SgTemplateParameterList *type_list, SgUnparse_Info& info);
          void unparseTypeArguments(SgTemplateParameterList *type_list, SgUnparse_Info& info);

          void unparseX10MarkerAnnotation(SgExpression *, SgUnparse_Info& info);
          void unparseX10SingleMemberAnnotation(SgExpression *, SgUnparse_Info& info);
          void unparseX10NormalAnnotation(SgExpression *, SgUnparse_Info& info);

          void unparseX10TypeExpression(SgExpression *, SgUnparse_Info& info);
          void unparseHereExpression(SgExpression *, SgUnparse_Info& info);
#endif
   };

#endif




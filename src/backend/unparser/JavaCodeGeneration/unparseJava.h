
#ifndef UNPARSER_JAVA
#define UNPARSER_JAVA

/* unparser.h
 * This header file contains the class declaration for the newest unparser. Six
 * C files include this header file: unparser.C, modified_sage.C, unparse_stmt.C, 
 * unparse_expr.C, unparse_type.C, and unparse_sym.C.
 */
#include "unparser.h"

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

#include "Cxx_Grammar.h"
#include "unparseLanguageIndependentConstructs.h"

// using namespace std;
class Unparser;

class JavaCodeGeneration_locatedNode : public UnparseLanguageIndependentConstructs
   {
     public:
          JavaCodeGeneration_locatedNode(Unparser* unp, std::string fname);

          virtual ~JavaCodeGeneration_locatedNode();

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
          virtual std::string languageName() const { return "C++ Unparser"; }

          virtual void unparseUnaryOperator           (SgExpression* expr, const char* op, SgUnparse_Info& info);
          virtual void unparseBinaryOperator          (SgExpression* expr, const char* op, SgUnparse_Info& info);
          virtual void unparseAssnExpr                (SgExpression* expr, SgUnparse_Info& info);   
          virtual void unparseVarRef                  (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseClassRef                (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseFuncRef                 (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseMFuncRef                (SgExpression* expr, SgUnparse_Info& info);

       // DQ (11/10/2005): Added general support for SgValue (so that we could unparse 
       // expression trees from contant folding)
          virtual void unparseStringVal               (SgExpression* expr, SgUnparse_Info& info);  

       // JJW -- these need C-specific suffixes
          virtual void unparseUIntVal                 (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseLongIntVal              (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseLongLongIntVal          (SgExpression* expr, SgUnparse_Info& info);    
          virtual void unparseULongLongIntVal         (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseULongIntVal             (SgExpression* expr, SgUnparse_Info& info); 
          virtual void unparseFloatVal                (SgExpression* expr, SgUnparse_Info& info); 
          virtual void unparseLongDoubleVal           (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseComplexVal              (SgExpression* expr, SgUnparse_Info& info);


          virtual void unparseFuncCall                (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparsePointStOp               (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseRecRef                  (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseDotStarOp               (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseArrowStarOp             (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseEqOp                    (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseLtOp                    (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseGtOp                    (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseNeOp                    (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseLeOp                    (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseGeOp                    (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseAddOp                   (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseSubtOp                  (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseMultOp                  (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseDivOp                   (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseIntDivOp                (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseModOp                   (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseAndOp                   (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseOrOp                    (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseBitXOrOp                (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseBitAndOp                (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseBitOrOp                 (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseCommaOp                 (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseLShiftOp                (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseRShiftOp                (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseJavaUnsignedRshiftOp    (SgExpression* expr, SgUnparse_Info& info);
          virtual void unparseJavaUnsignedRshiftAssignOp (SgExpression* expr, SgUnparse_Info& info);
          virtual void unparseUnaryMinusOp            (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseUnaryAddOp              (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseSizeOfOp                (SgExpression* expr, SgUnparse_Info& info);  

          virtual void unparseTypeIdOp                (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseNotOp                   (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseDerefOp                 (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseAddrOp                  (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseMinusMinusOp            (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparsePlusPlusOp              (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseAbstractOp              (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseBitCompOp               (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseRealPartOp              (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseImagPartOp              (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseConjugateOp             (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseExprCond                (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseClassInitOp             (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseDyCastOp                (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseCastOp                  (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseArrayOp                 (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseNewOp                   (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseDeleteOp                (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseThisNode                (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseScopeOp                 (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseAssnOp                  (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparsePlusAssnOp              (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseMinusAssnOp             (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseAndAssnOp               (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseIOrAssnOp               (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseMultAssnOp              (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseDivAssnOp               (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseModAssnOp               (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseXorAssnOp               (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseLShiftAssnOp            (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseRShiftAssnOp            (SgExpression* expr, SgUnparse_Info& info);  
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
          virtual void unparseJavaInstanceOfOp        (SgExpression* expr, SgUnparse_Info& info);

          virtual void unparseBasicBlockStmt   (SgStatement* stmt, SgUnparse_Info& info);

       // DQ (4/16/2011): Added Java specific "import" statement.
          virtual void unparseImportDeclarationStatement (SgStatement* stmt, SgUnparse_Info& info);

          virtual void unparseIfStmt           (SgStatement* stmt, SgUnparse_Info& info);
       // virtual void unparseWhereStmt        (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseForInitStmt      (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseForStmt          (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseFuncDeclStmt     (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseFuncDefnStmt     (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseMFuncDeclStmt    (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseVarDeclStmt      (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseVarDefnStmt      (SgStatement* stmt, SgUnparse_Info& info);
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
          virtual void unparseGotoStmt         (SgStatement* stmt, SgUnparse_Info& info);
       // virtual void unparseAsmStmt          (SgStatement* stmt, SgUnparse_Info& info);
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

      //! Convenience method for unparsing types using unp->u_type
          virtual void unparseType(SgType* type, SgUnparse_Info& info);

          virtual void curprint_indented(const std::string str, SgUnparse_Info& info) const;

          virtual void unparseNestedStatement(SgStatement* stmt, SgUnparse_Info& info);

          virtual PrecedenceSpecifier getPrecedence(SgExpression* exp);
          virtual AssociativitySpecifier getAssociativity(SgExpression* exp);
   };

#endif




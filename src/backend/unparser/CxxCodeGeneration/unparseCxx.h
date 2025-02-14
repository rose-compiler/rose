
/* unparser.h
 * This header file contains the class declaration for the newest unparser. Six
 * C files include this header file: unparser.C, modified_sage.C, unparse_stmt.C, 
 * unparse_expr.C, unparse_type.C, and unparse_sym.C.
 */

#ifndef UNPARSER_EXPRSTMT
#define UNPARSER_EXPRSTMT


#include "unparser.h"

// Unparser::token_sequence_position_enum_type xxx;

class SgExpression;
class SgStatement;
class SgOmpDefaultClause;
class SgOmpScheduleClause;
class SgOmpVariablesClause;
class SgOmpExpressionClause;
class SgTemplateParameter;
class SgTemplateArgument;
class SgFunctionDeclaration;
class SgBasicBlock;
class SgClassDefinition;
class SgTemplateInstantiationDecl;
class SgOmpClause;
class SgOmpBodyStatement;
class SgOmpThreadBodyStatement;
class SgOmpFlushStatement;
class SgOmpDeclareSimdStatement;
class SgOmpBarrierStatement;
class SgOmpTaskwaitStatement;
class SgNamespaceDefinitionStatement;
class SgAsmOp;

#include "Cxx_Grammar.h"

#include "unparseLanguageIndependentConstructs.h"

class Unparser;

class Unparse_ExprStmt : public UnparseLanguageIndependentConstructs
   {
     public:
          Unparse_ExprStmt(Unparser* unp, std::string fname);

          virtual ~Unparse_ExprStmt();

       // DQ (3/13/2004): Added to support templates
          virtual void unparseTemplateParameter(SgTemplateParameter* templateParameter, SgUnparse_Info& info, bool is_template_header=false);
          virtual void unparseTemplateArgument (SgTemplateArgument*  templateArgument , SgUnparse_Info& info);

       // DQ (11/27/2004): Added to support unparsing of pointers to nested template arguments 
          virtual void unparseTemplateName(SgTemplateInstantiationDecl* templateInstantiationDeclaration, SgUnparse_Info& info);

       // DQ (6/21/2011): Refactored support for unparing names with template arguments.
          void unparseTemplateFunctionName ( SgTemplateInstantiationFunctionDecl* templateInstantiationFunctionDeclaration, SgUnparse_Info& info );
          void unparseTemplateArgumentList ( const SgTemplateArgumentPtrList  & templateArgListPtr,    SgUnparse_Info& info );

       // DQ (9/13/2014): Added as part of refactoring support for name qualification.
          void unparseTemplateParameterList( const SgTemplateParameterPtrList & templateParameterList, SgUnparse_Info& info, bool is_template_header=false);

       // DQ (5/25/2013): Added support for unparsing the name of the template member function.
          void unparseTemplateMemberFunctionName ( SgTemplateInstantiationMemberFunctionDecl* templateInstantiationMemberFunctionDeclaration, SgUnparse_Info& info );

       // DQ (2/16/2004): Added to refactor code and add support for old-style K&R C
          void unparseFunctionArgs(SgFunctionDeclaration* funcdecl_stmt, SgUnparse_Info& info);
          void unparseFunctionParameterDeclaration ( SgFunctionDeclaration* funcdecl_stmt, 
                                                     SgInitializedName* initializedName,
                                                     bool outputParameterDeclaration, 
                                                     SgUnparse_Info& info );

          void unparse_helper(SgFunctionDeclaration* funcdecl_stmt, SgUnparse_Info& info);



       // DQ (5/27/2005): Added to support unparsing of compiler generated statements after comments 
       // attached to the following statement. We would like to not have any comments be attached to 
       // compiler generated statements so that they can be easily inserted anywhere.
      //! Unparser support for compiler-generated statments
       // void saveCompilerGeneratedStatements ( SgStatement* stmt, SgUnparse_Info & info );

      //! Unparser support for compiler-generated statments
          void outputCompilerGeneratedStatements( SgUnparse_Info & info );

      //! Support for Fortran numeric labels (can appear on any statement), this is an empty function for C/C++.
      //  virtual void unparseStatementNumbers ( SgStatement* stmt );

       // void output(SgLocatedNode* node);

       // DQ (12/21/2005): Added to support name qualification when explicitly stored in the AST
       // (rather than generated).

//        virtual void unparseQualifiedNameList(const SgQualifiedNamePtrList & qualifiedNameList);

       // DQ (8/14/2007): This is where all the langauge specific statement unparing is done
          virtual void unparseLanguageSpecificStatement (SgStatement*  stmt, SgUnparse_Info& info);
          virtual void unparseLanguageSpecificExpression(SgExpression* expr, SgUnparse_Info& info);

       // DQ (9/6/2010): Mark the derived class to support debugging.
          virtual std::string languageName() const { return "C++ Unparser"; }

      //! unparse expression functions implemented in unparse_expr.C
       // DQ (4/25/2005): Made this virtual so that Gabriel could build a specialized unparser.
       // virtual void unparseExpression      (SgExpression* expr, SgUnparse_Info& info);

          virtual void unparseUnaryOperator           (SgExpression* expr, const char* op, SgUnparse_Info& info);
          virtual void unparseBinaryOperator          (SgExpression* expr, const char* op, SgUnparse_Info& info);
       // virtual void unparseUnaryExpr               (SgExpression* expr, SgUnparse_Info& info);  
       // virtual void unparseBinaryExpr              (SgExpression* expr, SgUnparse_Info& info); 
          virtual void unparseAssnExpr                (SgExpression* expr, SgUnparse_Info& info);   
//        virtual void unparseExprRoot                (SgExpression* expr, SgUnparse_Info& info);  
//        virtual void unparseExprList                (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseVarRef                  (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseClassRef                (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseFuncRef                 (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseMFuncRef                (SgExpression* expr, SgUnparse_Info& info);

       // DQ (2/26/2012): Adding support for template member function calls.
          virtual void unparseTemplateFuncRef (SgExpression* expr, SgUnparse_Info& info);
          virtual void unparseTemplateMFuncRef(SgExpression* expr, SgUnparse_Info& info);

       // DQ (4/25/2012): Refactored support for different templated adn non-template member an non-member function references into these two template functions.
          template <class T> void unparseFuncRefSupport (SgExpression* expr, SgUnparse_Info& info);
          template <class T> void unparseMFuncRefSupport(SgExpression* expr, SgUnparse_Info& info);

          virtual void unparseStringVal               (SgExpression* expr, SgUnparse_Info& info);  

       // DQ (2/14/2019): Adding support for C++14 void values.
          virtual void unparseVoidValue               (SgExpression* expr, SgUnparse_Info& info);  

          // JJW -- these need C-specific suffixes
          virtual void unparseUIntVal                 (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseLongIntVal              (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseLongLongIntVal          (SgExpression* expr, SgUnparse_Info& info);    
          virtual void unparseULongLongIntVal         (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseULongIntVal             (SgExpression* expr, SgUnparse_Info& info); 
          virtual void unparseFloatVal                (SgExpression* expr, SgUnparse_Info& info); 
       // virtual void unparseDoubleVal               (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseLongDoubleVal           (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseComplexVal              (SgExpression* expr, SgUnparse_Info& info);
        // Liao, 6/18/2008  UPC support
          virtual void unparseUpcThreads              (SgExpression* expr, SgUnparse_Info& info);     
          virtual void unparseUpcMythread             (SgExpression* expr, SgUnparse_Info& info);     

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
          virtual void unparseUnaryMinusOp            (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseUnaryAddOp              (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseSizeOfOp                (SgExpression* expr, SgUnparse_Info& info);  

       // DQ (7/26/2020): Adding support for C++20 spaceship operator.
          virtual void unparseSpaceshipOp             (SgExpression* expr, SgUnparse_Info& info);

       // DQ (7/26/2020): Adding support for C++20 await expression.
          virtual void unparseAwaitExpression         (SgExpression* expr, SgUnparse_Info& info);

       // DQ (7/26/2020): Adding support for C++20 choose expression.
          virtual void unparseChooseExpression        (SgExpression* expr, SgUnparse_Info& info);

       // DQ (7/26/2020): Adding support for C++20 expression folding expression.
          virtual void unparseFoldExpression          (SgExpression* expr, SgUnparse_Info& info);

       // DQ (6/20/2013): Added support for C/C++ alignment extension __alignof__ operator.
          virtual void unparseAlignOfOp               (SgExpression* expr, SgUnparse_Info& info);  

       // DQ (2/5/2015): Added missing C++11 support.
          virtual void unparseNoexceptOp              (SgExpression* expr, SgUnparse_Info& info);  

          virtual void unparseScopedRefExp            (SgExpression* expr, SgUnparse_Info& info);
          virtual void unparseTypeRefExp              (SgExpression* expr, SgUnparse_Info& info);

       // DQ (2/12/2011): Added support for UPC specific sizeof operators.
          virtual void unparseUpcLocalSizeOfOp        (SgExpression* expr, SgUnparse_Info& info);
          virtual void unparseUpcBlockSizeOfOp        (SgExpression* expr, SgUnparse_Info& info);
          virtual void unparseUpcElemSizeOfOp         (SgExpression* expr, SgUnparse_Info& info);

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
          virtual void unparseCompInit                (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseCtorInit                 (SgExpression* expr, SgUnparse_Info& info);
          virtual void unparseAssnInit                (SgExpression* expr, SgUnparse_Info& info);

       // DQ (11/15/2016): Adding support for braced initializer node.
          virtual void unparseBracedInit              (SgExpression* expr, SgUnparse_Info& info);

          virtual void unparseThrowOp                 (SgExpression* expr, SgUnparse_Info& info);
          virtual void unparseVarArgStartOp           (SgExpression* expr, SgUnparse_Info& info);
          virtual void unparseVarArgStartOneOperandOp (SgExpression* expr, SgUnparse_Info& info);
          virtual void unparseVarArgOp                (SgExpression* expr, SgUnparse_Info& info);
          virtual void unparseVarArgEndOp             (SgExpression* expr, SgUnparse_Info& info);
          virtual void unparseVarArgCopyOp            (SgExpression* expr, SgUnparse_Info& info);
//        virtual void unparseNullExpression          (SgExpression* expr, SgUnparse_Info& info);
          virtual void unparseDesignatedInitializer   (SgExpression* expr, SgUnparse_Info& info);
          virtual void unparsePseudoDtorRef           (SgExpression* expr, SgUnparse_Info& info);

       // DQ (9/4/2013): Added support for compound literals.
          virtual void unparseCompoundLiteral         (SgExpression* expr, SgUnparse_Info& info);

       // DQ (7/24/2014): Added more general support for type expressions (required for C11 generic macro support.
          virtual void unparseTypeExpression          (SgExpression* expr, SgUnparse_Info& info);

       // DQ (8/11/2014): Added more general support for function parameter expressions (required for C++11 support).
          virtual void unparseFunctionParameterRefExpression (SgExpression* expr, SgUnparse_Info& info);

       // DQ (9/3/2014): Adding C++11 Lambda expression support.
          virtual void unparseLambdaExpression(SgExpression* expr, SgUnparse_Info& info);

       // DQ (11/21/2017): Adding support for GNU C extension for computed goto.
          virtual void unparseLabelRefExpression(SgExpression* expr, SgUnparse_Info& info);

          virtual void unparseNonrealRefExpression(SgExpression* expr, SgUnparse_Info& info);

      //! unparse statement functions implememted in unparse_stmt.C
       // DQ (4/25/2005): Made this virtual so that Gabriel could build a specialized unparser.
       // virtual void unparseStatement        (SgStatement* stmt, SgUnparse_Info& info);
       // virtual void unparseGlobalStmt       (SgStatement* stmt, SgUnparse_Info& info);
       // CI
       // virtual void unparseDeclStmt         (SgStatement* stmt, SgUnparse_Info& info);
//        virtual void unparseScopeStmt        (SgStatement* stmt, SgUnparse_Info& info);
//        virtual void unparseFuncTblStmt      (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseBasicBlockStmt   (SgStatement* stmt, SgUnparse_Info& info);
       // CI
       // virtual void unparseElseIfStmt       (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseIfStmt           (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseWhereStmt        (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseForInitStmt      (SgStatement* stmt, SgUnparse_Info& info);

          virtual void unparseForStmt          (SgStatement* stmt, SgUnparse_Info& info);

       // DQ (3/26/2018): Adding support for C++11 IR node (previously missed).
          virtual void unparseRangeBasedForStmt(SgStatement* stmt, SgUnparse_Info& info);

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
          virtual void unparseAsmStmt          (SgStatement* stmt, SgUnparse_Info& info);
//        virtual void unparseSpawnStmt        (SgStatement* stmt, SgUnparse_Info& info);
       // CI
       // virtual void unparseParStmt          (SgStatement* stmt, SgUnparse_Info& info);
       // CI
       // virtual void unparseParForStmt       (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseTypeDefStmt      (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseTemplateDeclStmt (SgStatement* stmt, SgUnparse_Info& info);
          
       // DQ (7/25/2014): Adding support for C11 static assertions.
          virtual void unparseStaticAssertionDeclaration (SgStatement* stmt, SgUnparse_Info& info);

       // DQ (8/17/2014): Adding support for Microsoft attributes.
          virtual void unparseMicrosoftAttributeDeclaration (SgStatement* stmt, SgUnparse_Info& info);

       // DQ (12/26/2011): New unparse functions for new template declaration IR nodes (new design for template declarations).
          virtual void unparseTemplateClassDeclStmt          (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseTemplateClassDefnStmt          (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseTemplateFunctionDeclStmt       (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseTemplateMemberFunctionDeclStmt (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseTemplateVariableDeclStmt       (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseTemplateFunctionDefnStmt       (SgStatement* stmt, SgUnparse_Info& info);

       // DQ 11/3/2014): Adding C++11 templated typedef declaration support.
          virtual void unparseTemplateTypedefDeclaration(SgStatement* stmt, SgUnparse_Info& info);

       // DQ (9/7/2014): Refactored this code so we could call it from the template member and non-member function declaration unparse function.
          virtual void unparseReturnType (SgFunctionDeclaration* funcdecl_stmt, SgType* & rtype, SgUnparse_Info& info);

       // DQ (9/9/2014): Refactoring support for member function modifiers (so that they can be used to unparse template membr functions).
          virtual void unparseTrailingFunctionModifiers(SgMemberFunctionDeclaration* mfuncdecl_stmt, SgUnparse_Info& info);

       // TV (3/12/18): Unparsing template headers, works the same way for class, function, methods (typedef and variable ???)
          template<class T> void unparseTemplateHeader(T* decl, SgUnparse_Info& info);

       // DQ (12/26/2011): Supporting function for all template declarations (initially at least).
          template<class T> void unparseTemplateDeclarationStatment_support(SgStatement* stmt, SgUnparse_Info& info);

          virtual void unparseNonrealDecl (SgStatement* stmt, SgUnparse_Info& info);

       // virtual void unparseNullStatement    (SgStatement* stmt, SgUnparse_Info& info);

       // DQ (7/21/2006): Added support for GNU statement expression extension.
          virtual void unparseStatementExpression (SgExpression* expr, SgUnparse_Info& info);

      // DQ (7/22/2006): Added support for asm operands.
         virtual void unparseAsmOp (SgExpression* expr, SgUnparse_Info& info);

      // DQ (6/25/2011): Can we modify this to perm it to be compiler withouth seeing the enum definition?
      // virtual void unparse_asm_operand_modifier(SgAsmOp::asm_operand_modifier_enum flags);
         virtual void unparse_asm_operand_modifier(SgAsmOp::asm_operand_modifier_enum flags);

       // DQ (2/29/2004): Added to support unparsing of template instantiations (similar to class declarations)
          virtual void unparseTemplateInstantiationDeclStmt               (SgStatement* stmt, SgUnparse_Info& info);

       // DQ (3/24/2004): Added to support template functions and template member functions
          virtual void unparseTemplateInstantiationFunctionDeclStmt       (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseTemplateInstantiationMemberFunctionDeclStmt (SgStatement* stmt, SgUnparse_Info& info);

       // DQ (4/16/2005): Added support for explicit template directives
          virtual void unparseTemplateInstantiationDirectiveStmt          (SgStatement* stmt, SgUnparse_Info& info);

          virtual void unparsePragmaDeclStmt   (SgStatement* stmt, SgUnparse_Info& info);

       // DQ (3/22/2019): Adding EmptyDeclaration to support addition of comments and CPP directives that will permit 
       // token-based unparsing to work with greater precision. For example, used to add an include directive with 
       // greater precision to the global scope and permit the unparsing via the token stream to be used as well.
          virtual void unparseEmptyDeclaration (SgStatement* stmt, SgUnparse_Info& info);

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

#if USE_OLD_MECHANISM_OF_HANDLING_PREPROCESSING_INFO
          void getDirectives ( char* sourceFilename );
#endif
//#if UPC_EXTENSIONS_ALLOWED
//#if USE_UPC_IR_NODES
       // Liao, 6/13/2008, support UPC nodes
          virtual void unparseUpcNotifyStatement  (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseUpcWaitStatement    (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseUpcBarrierStatement (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseUpcFenceStatement   (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseUpcForAllStatement  (SgStatement* stmt, SgUnparse_Info& info);
//#endif       
       // Liao 5/31/2009, OpenMP nodes
         virtual void unparseOmpPrefix                     (SgUnparse_Info& info); 
//       virtual void unparseOmpDefaultClause(SgOmpDefaultClause* clause, SgUnparse_Info& info);
//       virtual void unparseOmpScheduleClause(SgOmpScheduleClause* clause, SgUnparse_Info& info);
//       virtual void unparseOmpVariablesClause(SgOmpVariablesClause* clause, SgUnparse_Info& info);
//       virtual void unparseOmpExpressionClause(SgOmpExpressionClause* clause, SgUnparse_Info& info);
//       virtual void unparseOmpClause(SgOmpClause* clause, SgUnparse_Info& info);
//       virtual void unparseOmpParallelStatement (SgStatement* stmt, SgUnparse_Info& info);
//       virtual void unparseOmpBodyStatement(SgOmpBodyStatement* stmt, SgUnparse_Info& info);
//       virtual void unparseOmpThreadprivateStatement(SgOmpThreadprivateStatement* stmt, SgUnparse_Info& info);
//       virtual void unparseOmpFlushStatement(SgOmpFlushStatement* stmt, SgUnparse_Info& info);
//       virtual void unparseOmpBarrierStatement(SgOmpBarrierStatement* stmt, SgUnparse_Info& info);
//       virtual void unparseOmpTaskwaitStatement(SgOmpTaskwaitStatement* stmt, SgUnparse_Info& info);

         virtual void unparseOmpForStatement          (SgStatement* stmt, SgUnparse_Info& info);
         virtual void unparseOmpForSimdStatement      (SgStatement* stmt, SgUnparse_Info& info);
         virtual void unparseOmpBeginDirectiveClauses (SgStatement* stmt, SgUnparse_Info& info);
       // DQ (8/13/2007): This should go into the Unparser class
      //! begin the unparser (unparser.C)
       // void run_unparser();
       
       // TV (05/06/2010): SgCudaKernelCallExp
          virtual void unparseCudaKernelCall(SgExpression* expr, SgUnparse_Info& info);

       // DQ (7/21/2012): New IR node (only seen in C++11 so far).
          void unparseTemplateParameterValue(SgExpression* expr, SgUnparse_Info& info);

       // DQ (10/5/2012): Added support for GNU type attributes.
          void unparseTypeAttributes ( SgDeclarationStatement* declaration );

       // DQ (5/24/2013): Added to support refactoing of the code to unparse the template arguments when
       // they are generated seperately for SgFunctionRefExp and SgMemberFunctionRefExp IR nodes.
       // void unparseGeneratedTemplateArgumentsList (SgName unqualifiedName, SgName qualifiedName, SgLocatedNode* locatedNode, SgUnparse_Info& info);

       // DQ (7/12/2013): Added support for type trait builtin functions.
          void unparseTypeTraitBuiltinOperator(SgExpression* expr, SgUnparse_Info& info);

       // DQ (2/8/2014): The name of the constructor call for a SgConstructorInitializer must 
       // be output differently for the GNU g++ 4.5 version compiler and later.
          SgName trimOutputOfFunctionNameForGNU_4_5_VersionAndLater(SgName nameQualifier, bool & skipOutputOfFunctionName);

       // DQ (1/21/2018): Added support for lambda function capture variables in annonymous compiler generated classes passed as template arguments
          bool isAnonymousClass(SgType* templateArgumentType);

       // DQ (1/8/2020): Refactors the output of base classes so that it can be supported in the unparseClassDefnStmt() and unparseClassType() functions.
          void unparseClassInheritanceList ( SgClassDefinition* classdefn_stmt, SgUnparse_Info& ninfo );

       // DQ (9/23/2020): Output the SgPragma when it is associated with a SgScopeStatement.
          void unparsePragmaAttribute(SgScopeStatement* scope_stmt);
   };

// Helpers

std::string unparse_register_name (SgInitializedName::asm_register_name_enum register_name);

#endif



/* unparser.h
 * This header file contains the class declaration for the newest unparser. Six
 * C files include this header file: unparser.C, modified_sage.C, unparse_stmt.C, 
 * unparse_expr.C, unparse_type.C, and unparse_sym.C.
 */

#ifndef UNPARSER_EXPRSTMT
#define UNPARSER_EXPRSTMT


#include "unparser.h"


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
class SgOmpBarrierStatement;
class SgOmpTaskwaitStatement;
class SgNamespaceDefinitionStatement;
class SgAsmOp;

#include "Cxx_Grammar.h"


#include "unparseLanguageIndependentConstructs.h"

// using namespace std;
class Unparser;

class Unparse_ExprStmt : public UnparseLanguageIndependentConstructs
   {
#if 0
  // DQ (8/13/2007): This should go into the base class
     private:
          Unparser* unp;
          std::string currentOutputFileName;
#endif
     public:
       // Unparse_ExprStmt(Unparser* unp, string fname) : unp(unp)
       // Unparse_ExprStmt(Unparser* unp, std::string fname) : UnparseLanguageIndependentConstructs(unp,fname) {};
          Unparse_ExprStmt(Unparser* unp, std::string fname);

          virtual ~Unparse_ExprStmt();

       // DQ (3/13/2004): Added to support templates
          virtual void unparseTemplateParameter(SgTemplateParameter* templateParameter, SgUnparse_Info& info);
          virtual void unparseTemplateArgument (SgTemplateArgument*  templateArgument , SgUnparse_Info& info);

       // DQ (11/27/2004): Added to support unparsing of pointers to nested template arguments 
          virtual void unparseTemplateName(SgTemplateInstantiationDecl* templateInstantiationDeclaration, SgUnparse_Info& info);

       // DQ (2/16/2004): Added to refactor code and add support for old-style K&R C
          void unparseFunctionArgs(SgFunctionDeclaration* funcdecl_stmt, SgUnparse_Info& info);
          void unparseFunctionParameterDeclaration ( SgFunctionDeclaration* funcdecl_stmt, 
                                                     SgInitializedName* initializedName,
                                                     bool outputParameterDeclaration, 
                                                     SgUnparse_Info& info );

          void unparseOneElemConInit(SgConstructorInitializer* con_init, SgUnparse_Info& info);

          void unparse_helper(SgFunctionDeclaration* funcdecl_stmt, SgUnparse_Info& info);

#if 0
       // DQ (8/13/2007): This should go into the base class

      //! get the filename from a Sage Statement Object
       // const char* getFileName(SgNode* stmt);
          std::string getFileName(SgNode* stmt);
  
      //! get the filename from the Sage File Object
       // char* getFileName();
          std::string getFileName();

      //! used to support the run_unparser function
      //! (support for #line 42 "filename" when it appears in source code)
       // bool statementFromFile ( SgStatement* stmt, char* sourceFilename );
          bool statementFromFile ( SgStatement* stmt, std::string sourceFilename );

      //! Generate a CPP directive  
          void outputDirective ( PreprocessingInfo* directive );

  
      //! counts the number of statements in a basic block
          int num_stmt_in_block(SgBasicBlock*);
#endif

       // DQ (5/27/2005): Added to support unparsing of compiler generated statements after comments 
       // attached to the following statement. We would like to not have any comments be attached to 
       // compiler generated statements so that they can be easily inserted anywhere.
      //! Unparser support for compiler-generated statments
       // void saveCompilerGeneratedStatements ( SgStatement* stmt, SgUnparse_Info & info );

      //! Unparser support for compiler-generated statments
          void outputCompilerGeneratedStatements( SgUnparse_Info & info );

#if 0
#if USE_OLD_MECHANISM_OF_HANDLING_PREPROCESSING_INFO

#error "DEAD CODE!: I think this is OLD code that can be removed."

      //! functions that unparses directives and/or comments
          void unparseDirectives(SgStatement* stmt);
          void unparseFinalDirectives ( char* filename );
  
          void unparseDirectivesUptoButNotIncludingCurrentStatement ( SgStatement* stmt );
          void unparseDirectivesSharingLineWithStatement ( SgStatement* stmt );
  
       // lower level member function called by unparseDirectives() and unparseFinalDirectives()
          void unparseDirectives ( char* currentFilename, int currentPositionInListOfDirectives, int currentStatementLineNumber );
#else
      //! This is the new member function
          virtual void unparseAttachedPreprocessingInfo(SgStatement* stmt, SgUnparse_Info& info,
                                                PreprocessingInfo::RelativePositionType whereToUnparse);
#endif
#endif
  

#if 0  
       // DQ (8/13/2007): This should go into the base class
          bool RemoveArgs(SgExpression* expr);
#endif

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

       // DQ (11/10/2005): Added general support for SgValue (so that we could unparse 
       // expression trees from contant folding)
       // virtual void unparseValue                   (SgExpression* expr, SgUnparse_Info& info);
#if 0
          virtual void unparseBoolVal                 (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseShortVal                (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseCharVal                 (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseUCharVal                (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseWCharVal                (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseStringVal               (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseUShortVal               (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseEnumVal                 (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseIntVal                  (SgExpression* expr, SgUnparse_Info& info);     
#else
          virtual void unparseStringVal               (SgExpression* expr, SgUnparse_Info& info);  
#endif
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
          virtual void unparseVarArgStartOp           (SgExpression* expr, SgUnparse_Info& info);
          virtual void unparseVarArgStartOneOperandOp (SgExpression* expr, SgUnparse_Info& info);
          virtual void unparseVarArgOp                (SgExpression* expr, SgUnparse_Info& info);
          virtual void unparseVarArgEndOp             (SgExpression* expr, SgUnparse_Info& info);
          virtual void unparseVarArgCopyOp            (SgExpression* expr, SgUnparse_Info& info);
//        virtual void unparseNullExpression          (SgExpression* expr, SgUnparse_Info& info);
          virtual void unparseDesignatedInitializer   (SgExpression* expr, SgUnparse_Info& info);
          virtual void unparsePseudoDtorRef           (SgExpression* expr, SgUnparse_Info& info);

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
//        virtual void unparseNullStatement    (SgStatement* stmt, SgUnparse_Info& info);

       // DQ (7/21/2006): Added support for GNU statement expression extension.
          virtual void unparseStatementExpression (SgExpression* expr, SgUnparse_Info& info);

      // DQ (7/22/2006): Added support for asm operands.
         virtual void unparseAsmOp (SgExpression* expr, SgUnparse_Info& info);
         virtual void unparse_asm_operand_modifier(SgAsmOp::asm_operand_modifier_enum flags);
         std::string unparse_register_name (SgInitializedName::asm_register_name_enum register_name);

       // DQ (2/29/2004): Added to support unparsing of template instantiations (similar to class declarations)
          virtual void unparseTemplateInstantiationDeclStmt               (SgStatement* stmt, SgUnparse_Info& info);

       // DQ (3/24/2004): Added to support template functions and template member functions
          virtual void unparseTemplateInstantiationFunctionDeclStmt       (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseTemplateInstantiationMemberFunctionDeclStmt (SgStatement* stmt, SgUnparse_Info& info);

       // DQ (4/16/2005): Added support for explicit template directives
          virtual void unparseTemplateInstantiationDirectiveStmt          (SgStatement* stmt, SgUnparse_Info& info);

          virtual void unparsePragmaDeclStmt   (SgStatement* stmt, SgUnparse_Info& info);

#if 0
       // DQ (8/13/2007): This should go into the base class
          bool isTransformed(SgStatement* stmt);
          void markGeneratedFile();

       // DQ (8/13/2007): This function was added by Thomas to replace (wrap) the use of cur as an output stream.
          void curprint (std::string str);
#endif

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
       virtual void unparseUpcNotifyStatement (SgStatement* stmt, SgUnparse_Info& info);
       virtual void unparseUpcWaitStatement (SgStatement* stmt, SgUnparse_Info& info);
       virtual void unparseUpcBarrierStatement (SgStatement* stmt, SgUnparse_Info& info);
       virtual void unparseUpcFenceStatement (SgStatement* stmt, SgUnparse_Info& info);
       virtual void unparseUpcForAllStatement (SgStatement* stmt, SgUnparse_Info& info);
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
         virtual void unparseOmpBeginDirectiveClauses (SgStatement* stmt, SgUnparse_Info& info);
       // DQ (8/13/2007): This should go into the Unparser class
      //! begin the unparser (unparser.C)
       // void run_unparser();
       
       // TV (05/06/2010): SgCudaKernelCallExp
       virtual void unparseCudaKernelCall(SgExpression* expr, SgUnparse_Info& info);

};

#endif



#if 0
// DQ (8/13/2007): This code was previously in the class above.

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
  static std::string removeUnwantedWhiteSpace ( const std::string & X );


  //! unparse symbol functions implemented in unparse_sym.C
  // DQ (4/25/2005): Made this virtual so that Gabriel could build a specialized unparser.
  virtual void unparseSymbol(SgSymbol* sym, SgUnparse_Info& info);
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
#endif
 

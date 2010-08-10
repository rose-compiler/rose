
/* unparser.h
 * This header file contains the class declaration for the newest unparser. Six
 * C files include this header file: unparser.C, modified_sage.C, unparse_stmt.C, 
 * unparse_expr.C, unparse_type.C, and unparse_sym.C.
 */

#ifndef UNPARSER_FORTRAN
#define UNPARSER_FORTRAN

#include "unparser.h"

class FortranCodeGeneration_locatedNode : public UnparseLanguageIndependentConstructs
   {
     public:
       // FortranCodeGeneration_locatedNode(Unparser* unp, string fname) : UnparseLanguageIndependentConstructs(unp,fname) {};
          FortranCodeGeneration_locatedNode(Unparser* unp, std::string fname);

          virtual ~FortranCodeGeneration_locatedNode();

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

      //! Support for Fortran include mechanism (not a Fortran statement in the grammar)
          virtual void unparseFortranIncludeLine ( SgStatement* stmt, SgUnparse_Info& info );

      //! Support for Fortran numeric labels (can appear on any statement), this is an empty function for C/C++.
          virtual void unparseStatementNumbers ( SgStatement* stmt, SgUnparse_Info& info );
       // virtual void unparseStatementNumbers ( SgStatement* stmt );

      //! Supporting function for unparseStatementNumbers and for direct use in unparsing statments that have an end_numeric_label field.
          void unparseStatementNumbersSupport ( SgLabelRefExp* numeric_label_exp, SgUnparse_Info& info );
       // void unparseStatementNumbersSupport ( SgLabelSymbol* numeric_label_symbol, SgUnparse_Info& info );
       // void unparseStatementNumbersSupport ( SgLabelSymbol* numeric_label );
       // void unparseStatementNumbersSupport ( int value );

       // DQ (8/15/2007): Build a fortran specific version of this function.
          void unparse_helper (SgFunctionDeclaration* funcdecl_stmt, SgUnparse_Info& info);

          void unparseFunctionArgs(SgFunctionDeclaration* funcdecl_stmt, SgUnparse_Info& info);
          void unparseFunctionParameterDeclaration ( SgFunctionDeclaration* funcdecl_stmt, 
                                                     SgInitializedName* initializedName,
                                                     bool outputParameterDeclaration, 
                                                     SgUnparse_Info& info );

       // DQ (12/21/2005): Added to support name qualification when explicitly stored in the AST
       // (rather than generated).
       // virtual void unparseQualifiedNameList(const SgQualifiedNamePtrList & qualifiedNameList);

       // DQ (8/14/2007): This is where all the langauge specific statement unparing is done
          virtual void unparseLanguageSpecificStatement (SgStatement*  stmt, SgUnparse_Info& info);
          virtual void unparseLanguageSpecificExpression(SgExpression* expr, SgUnparse_Info& info);

      //! unparse expression functions implemented in unparse_expr.C
       // DQ (4/25/2005): Made this virtual so that Gabriel could build a specialized unparser.
       // virtual void unparseExpression              (SgExpression* expr, SgUnparse_Info& info);

          virtual void unparseUnaryOperator           (SgExpression* expr, const char* op, SgUnparse_Info& info);
          virtual void unparseBinaryOperator          (SgExpression* expr, const char* op, SgUnparse_Info& info);
       // virtual void unparseUnaryExpr               (SgExpression* expr, SgUnparse_Info& info);
       // virtual void unparseBinaryExpr              (SgExpression* expr, SgUnparse_Info& info);
//        virtual void unparseAssnExpr                (SgExpression* expr, SgUnparse_Info& info);
          virtual void unparseExprRoot                (SgExpression* expr, SgUnparse_Info& info);

//        virtual void unparseExprList                (SgExpression* expr, SgUnparse_Info& info);
       // DQ (8/14/2007): Accept the use of the bool for now, but swith to information communicated through info object!
       // This function is different from the base class version (and it should be the same).
          virtual void unparseExprList                (SgExpression* expr, SgUnparse_Info& info, bool paren);

          virtual void unparseVarRef                  (SgExpression* expr, SgUnparse_Info& info);
          virtual void unparseClassRef                (SgExpression* expr, SgUnparse_Info& info);
          virtual void unparseFuncRef                 (SgExpression* expr, SgUnparse_Info& info);
          virtual void unparseMFuncRef                (SgExpression* expr, SgUnparse_Info& info);

       // DQ (11/10/2005): Added general support for SgValue (so that we could unparse 
       // expression trees from contant folding)
       // virtual void unparseValue                   (SgExpression* expr, SgUnparse_Info& info);
#if 0
          virtual void unparseShortVal                (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseCharVal                 (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseUCharVal                (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseWCharVal                (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseStringVal               (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseUShortVal               (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseEnumVal                 (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseIntVal                  (SgExpression* expr, SgUnparse_Info& info);     
          virtual void unparseUIntVal                 (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseLongIntVal              (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseLongLongIntVal          (SgExpression* expr, SgUnparse_Info& info);    
          virtual void unparseULongLongIntVal         (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseULongIntVal             (SgExpression* expr, SgUnparse_Info& info); 
          virtual void unparseFloatVal                (SgExpression* expr, SgUnparse_Info& info); 
          virtual void unparseDoubleVal               (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseLongDoubleVal           (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseComplexVal              (SgExpression* expr, SgUnparse_Info& info);
#else
          virtual void unparseStringVal               (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseBoolVal                 (SgExpression* expr, SgUnparse_Info& info);  
#endif
          virtual void unparseFuncCall                (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparsePointStOp               (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseRecRef                  (SgExpression* expr, SgUnparse_Info& info);  
//        virtual void unparseDotStarOp               (SgExpression* expr, SgUnparse_Info& info);  
//        virtual void unparseArrowStarOp             (SgExpression* expr, SgUnparse_Info& info);  
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
//        virtual void unparseCommaOp                 (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseLShiftOp                (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseRShiftOp                (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseUnaryMinusOp            (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseUnaryAddOp              (SgExpression* expr, SgUnparse_Info& info);  
//        virtual void unparseSizeOfOp                (SgExpression* expr, SgUnparse_Info& info);  
//        virtual void unparseTypeIdOp                (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseNotOp                   (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseDerefOp                 (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseAddrOp                  (SgExpression* expr, SgUnparse_Info& info);  
//        virtual void unparseMinusMinusOp            (SgExpression* expr, SgUnparse_Info& info);  
//        virtual void unparsePlusPlusOp              (SgExpression* expr, SgUnparse_Info& info);  
//        virtual void unparseAbstractOp              (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseBitCompOp               (SgExpression* expr, SgUnparse_Info& info);  
//        virtual void unparseExprCond                (SgExpression* expr, SgUnparse_Info& info);  
//        virtual void unparseClassInitOp             (SgExpression* expr, SgUnparse_Info& info);  
//        virtual void unparseDyCastOp                (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseCastOp                  (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseArrayOp                 (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseNewOp                   (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseDeleteOp                (SgExpression* expr, SgUnparse_Info& info);  
//        virtual void unparseThisNode                (SgExpression* expr, SgUnparse_Info& info);  
//        virtual void unparseScopeOp                 (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseAssnOp                  (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparsePointerAssnOp           (SgExpression* expr, SgUnparse_Info& info);  
//        virtual void unparsePlusAssnOp              (SgExpression* expr, SgUnparse_Info& info);  
//        virtual void unparseMinusAssnOp             (SgExpression* expr, SgUnparse_Info& info);  
//        virtual void unparseAndAssnOp               (SgExpression* expr, SgUnparse_Info& info);  
//        virtual void unparseIOrAssnOp               (SgExpression* expr, SgUnparse_Info& info);  
//        virtual void unparseMultAssnOp              (SgExpression* expr, SgUnparse_Info& info);  
//        virtual void unparseDivAssnOp               (SgExpression* expr, SgUnparse_Info& info);  
//        virtual void unparseModAssnOp               (SgExpression* expr, SgUnparse_Info& info);  
//        virtual void unparseXorAssnOp               (SgExpression* expr, SgUnparse_Info& info);  
//        virtual void unparseLShiftAssnOp            (SgExpression* expr, SgUnparse_Info& info);  
//        virtual void unparseRShiftAssnOp            (SgExpression* expr, SgUnparse_Info& info);  
//        virtual void unparseForDeclOp               (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseTypeRef                 (SgExpression* expr, SgUnparse_Info& info);  
//        virtual void unparseVConst                  (SgExpression* expr, SgUnparse_Info& info);  
//        virtual void unparseExprInit                (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseInitializerList         (SgExpression* expr, SgUnparse_Info& info);
          virtual void unparseAggrInit                (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseConInit                 (SgExpression* expr, SgUnparse_Info& info);
          virtual void unparseAssnInit                (SgExpression* expr, SgUnparse_Info& info);
//        virtual void unparseThrowOp                 (SgExpression* expr, SgUnparse_Info& info);
//        virtual void unparseVarArgStartOp           (SgExpression* expr, SgUnparse_Info& info);
//        virtual void unparseVarArgStartOneOperandOp (SgExpression* expr, SgUnparse_Info& info);
//        virtual void unparseVarArgOp                (SgExpression* expr, SgUnparse_Info& info);
//        virtual void unparseVarArgEndOp             (SgExpression* expr, SgUnparse_Info& info);
//        virtual void unparseVarArgCopyOp            (SgExpression* expr, SgUnparse_Info& info);
       // virtual void unparseNullExpression          (SgExpression* expr, SgUnparse_Info& info);


       // DQ (8/14/2007): Fortran specific member functions
       // virtual void unparseIntrinsic                (SgExpression* expr, SgUnparse_Info& info);
          virtual void unparseExpOp                    (SgExpression* expr, SgUnparse_Info& info);
          virtual void unparseSubscriptExpr            (SgExpression* expr, SgUnparse_Info& info);
       // virtual void unparseSubColon                 (SgExpression* expr, SgUnparse_Info& info);

       // DQ (11/18/2007): Added support for ":" in declarations
          virtual void unparseColonShapeExp            (SgExpression* expr, SgUnparse_Info& info);
       // virtual void unparseSubAsterick              (SgExpression* expr, SgUnparse_Info& info);
          virtual void unparseAsteriskShapeExp         (SgExpression* expr, SgUnparse_Info& info);

       // DQ (10/4/2008): I no longer agree that these are expressions, they are just parts of the SgUseStatment.
       // virtual void unparseUseRename                (SgExpression* expr, SgUnparse_Info& info);
       // virtual void unparseUseOnly                  (SgExpression* expr, SgUnparse_Info& info);

          virtual void unparseIOItemExpr               (SgExpression* expr, SgUnparse_Info& info);
          virtual void unparseImpliedDo                (SgExpression* expr, SgUnparse_Info& info);

          virtual void unparseConcatenationOp          (SgExpression* expr, SgUnparse_Info& info);

       // DQ (11/24/2007): These will be translated away in a post-processing step, but allow them to be unparsed for debugging
          virtual void unparseUnknownArrayOrFunctionReference (SgExpression* expr, SgUnparse_Info& info);

          virtual bool isSubroutineCall                (SgFunctionCallExp* fcall);

       // DQ (10/10/2008): Added support for unser defined unary and binary operators.
          virtual void unparseUserDefinedUnaryOp       (SgExpression* expr, SgUnparse_Info& info);
          virtual void unparseUserDefinedBinaryOp      (SgExpression* expr, SgUnparse_Info& info);

          virtual void unparseModuleStmt               (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseProgHdrStmt              (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseProcHdrStmt              (SgStatement* stmt, SgUnparse_Info& info);
          virtual void genPUAutomaticStmts             (SgStatement* stmt, SgUnparse_Info& info);

          virtual void unparseInterfaceStmt            (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseCommonBlock              (SgStatement* stmt, SgUnparse_Info& info);

          virtual void unparseFuncArgs                 (SgInitializedNamePtrList* args, SgUnparse_Info& info);
          virtual void unparseInitNamePtrList          (SgInitializedNamePtrList* args, SgUnparse_Info& info);
          virtual void printAccessModifier             (SgDeclarationStatement * decl_stmt, SgUnparse_Info & info);

          virtual void printStorageModifier            (SgDeclarationStatement* decl_stmt, SgUnparse_Info& info);
          virtual void unparseVarDecl                  (SgStatement* stmt, SgInitializedName* initializedName, SgUnparse_Info& info);
          virtual void unparseParamDeclStmt            (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseUseStmt                  (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseDoStmt                   (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseStopOrPauseStmt          (SgStatement* stmt, SgUnparse_Info& info);

       // virtual void unparseIOStmt                   (SgStatement* stmt, SgUnparse_Info& info);
       // virtual void unparse_IO_ControlStatement     (SgStatement* stmt, SgUnparse_Info& info);

          virtual void unparsePrintStatement           (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseReadStatement            (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseWriteStatement           (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseOpenStatement            (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseCloseStatement           (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseInquireStatement         (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseFlushStatement           (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseRewindStatement          (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseBackspaceStatement       (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseEndfileStatement         (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseWaitStatement            (SgStatement* stmt, SgUnparse_Info& info);

          virtual void unparseAssociateStatement       (SgStatement* stmt, SgUnparse_Info& info);

       // Unparse base class data members
          virtual void unparse_IO_Support              (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparse_IO_Control_Support      ( std::string name, SgExpression* expr, SgUnparse_Info& info);

       // virtual void unparseInOutStmt                (SgStatement* stmt, SgUnparse_Info& info);
       // virtual void unparseIOFileControlStatement   (SgStatement* stmt, SgUnparse_Info& info);

          virtual void printDeclModifier               (SgDeclarationStatement* decl_stmt, SgUnparse_Info & info);

          virtual void unparseImplicitStmt             (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseBlockDataStmt            (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseStatementFunctionStmt    (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseWhereStmt                (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseElseWhereStmt            (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseNullifyStmt              (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseEquivalenceStmt          (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseArithmeticIfStmt         (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseAssignStmt               (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseComputedGotoStmt         (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseAssignedGotoStmt         (SgStatement* stmt, SgUnparse_Info& info);


      //! unparse statement functions implememted in unparse_stmt.C
       // DQ (4/25/2005): Made this virtual so that Gabriel could build a specialized unparser.
       // virtual void unparseStatement        (SgStatement* stmt, SgUnparse_Info& info);
       // virtual void unparseGlobalStmt       (SgStatement* stmt, SgUnparse_Info& info);
       // CI          
       // virtual void unparseDeclStmt         (SgStatement* stmt, SgUnparse_Info& info);
//        virtual void unparseScopeStmt        (SgStatement* stmt, SgUnparse_Info& info);
       // virtual void unparseFuncTblStmt      (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseBasicBlockStmt   (SgStatement* stmt, SgUnparse_Info& info);
       // CI
       // virtual void unparseElseIfStmt       (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseIfStmt           (SgStatement* stmt, SgUnparse_Info& info);
//        virtual void unparseWhereStmt        (SgStatement* stmt, SgUnparse_Info& info);
//        virtual void unparseForInitStmt      (SgStatement* stmt, SgUnparse_Info& info);
//        virtual void unparseForStmt          (SgStatement* stmt, SgUnparse_Info& info);
//        virtual void unparseFuncDeclStmt     (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseFuncDefnStmt     (SgStatement* stmt, SgUnparse_Info& info);
//        virtual void unparseMFuncDeclStmt    (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseVarDeclStmt      (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseVarDefnStmt      (SgStatement* stmt, SgUnparse_Info& info);
       // virtual void unparseClassDeclStmt    (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseClassDeclStmt_module      (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseClassDeclStmt_derivedType (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseClassDefnStmt    (SgStatement* stmt, SgUnparse_Info& info);
//        virtual void unparseEnumDeclStmt     (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseExprStmt         (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseLabelStmt        (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseWhileStmt        (SgStatement* stmt, SgUnparse_Info& info);
//        virtual void unparseDoWhileStmt      (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseSwitchStmt       (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseCaseStmt         (SgStatement* stmt, SgUnparse_Info& info);
//        virtual void unparseTryStmt          (SgStatement* stmt, SgUnparse_Info& info);
//        virtual void unparseCatchStmt        (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseDefaultStmt      (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseBreakStmt        (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseContinueStmt     (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseReturnStmt       (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseGotoStmt         (SgStatement* stmt, SgUnparse_Info& info);
//        virtual void unparseAsmStmt          (SgStatement* stmt, SgUnparse_Info& info);
//        virtual void unparseSpawnStmt        (SgStatement* stmt, SgUnparse_Info& info);
       // CI
       // virtual void unparseParStmt          (SgStatement* stmt, SgUnparse_Info& info);
       // CI
       // virtual void unparseParForStmt       (SgStatement* stmt, SgUnparse_Info& info);
//        virtual void unparseTypeDefStmt      (SgStatement* stmt, SgUnparse_Info& info);
//        virtual void unparseTemplateDeclStmt (SgStatement* stmt, SgUnparse_Info& info);
//        virtual void unparseNullStatement    (SgStatement* stmt, SgUnparse_Info& info);

       // DQ (11/17/2007): support for type attributes when used as statements.
          virtual void unparseAttributeSpecificationStatement (SgStatement* stmt, SgUnparse_Info& info);

       // DQ (11/19/2007): support for type attributes when used as statements.
          virtual void unparseNamelistStatement (SgStatement* stmt, SgUnparse_Info& info);

       // DQ (11/21/2007): support for import statement (F2003)
          virtual void unparseImportStatement  (SgStatement* stmt, SgUnparse_Info& info);

       // DQ (11/23/2007): support for bind attributes for declarations (F2003)
          virtual void unparseBindAttribute    (SgDeclarationStatement* declaration );

          virtual void unparseFormatStatement  (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseFormatItemList   (SgFormatItemList* formatItemList, SgUnparse_Info& info);

          virtual void unparseForAllStatement  (SgStatement* stmt, SgUnparse_Info& info);

          virtual void unparseContainsStatement(SgStatement* stmt, SgUnparse_Info& info);

          virtual void unparseEntryStatement   (SgStatement* stmt, SgUnparse_Info& info);

       // DQ (7/21/2006): Added support for GNU statement expression extension.
//        virtual void unparseStatementExpression (SgExpression* expr, SgUnparse_Info& info);

      // DQ (7/22/2006): Added support for asm operands.
//       virtual void unparseAsmOp (SgExpression* expr, SgUnparse_Info& info);
//       virtual void unparse_asm_operand_modifier(SgAsmOp::asm_operand_modifier_enum flags);
//       std::string unparse_register_name (SgInitializedName::asm_register_name_enum register_name);

       // DQ (2/29/2004): Added to support unparsing of template instantiations (similar to class declarations)
//        virtual void unparseTemplateInstantiationDeclStmt               (SgStatement* stmt, SgUnparse_Info& info);

       // DQ (3/24/2004): Added to support template functions and template member functions
//        virtual void unparseTemplateInstantiationFunctionDeclStmt       (SgStatement* stmt, SgUnparse_Info& info);
//        virtual void unparseTemplateInstantiationMemberFunctionDeclStmt (SgStatement* stmt, SgUnparse_Info& info);

       // DQ (4/16/2005): Added support for explicit template directives
//        virtual void unparseTemplateInstantiationDirectiveStmt          (SgStatement* stmt, SgUnparse_Info& info);

          virtual void unparsePragmaDeclStmt   (SgStatement* stmt, SgUnparse_Info& info);

       // DQ (5/1/2004): Added support for unparsing namespace constructs
//        virtual void unparseNamespaceDeclarationStatement      ( SgStatement* stmt, SgUnparse_Info & info );
//        virtual void unparseNamespaceDefinitionStatement       ( SgStatement* stmt, SgUnparse_Info & info );
//        virtual void unparseNamespaceAliasDeclarationStatement ( SgStatement* stmt, SgUnparse_Info & info );
//        virtual void unparseUsingDirectiveStatement            ( SgStatement* stmt, SgUnparse_Info & info );
//        virtual void unparseUsingDeclarationStatement          ( SgStatement* stmt, SgUnparse_Info & info );

       // DQ (10/14/2004): Supporting function shared by unparseClassDecl and unparseClassType
          void initializeDeclarationsFromParent ( SgDeclarationStatement* declarationStatement,
                                                  SgClassDefinition* & cdefn,
                                                  SgNamespaceDefinitionStatement* & namespaceDefn, int debugSupport = 0 );

       // DQ (12/1/2007): Need to handle kind and type parameters.
          void unparseModifierType(SgType* type, SgUnparse_Info& info);

       // DQ (12/9/2007): Added support for output of labels (to support arithmetic if)
          void unparseLabel ( SgLabelRefExp* exp );
       // void unparseLabel ( SgLabelSymbol* symbol );

          virtual void unparseLabelRefExp(SgExpression* expr, SgUnparse_Info& info);

          virtual void unparseActualArgumentExpression(SgExpression* expr, SgUnparse_Info& info);

#if 0
      //! DQ (10/12/2006): Support for qualified names (function names can't have global scope specifier in GNU, or so it seems).
          std::string trimGlobalScopeQualifier ( std::string qualifiedName );
#endif
#if 0
      //! Support for exception specification for functions and member functions
          virtual void unparseExceptionSpecification( SgTypePtrList* exceptionSpecifierList, SgUnparse_Info& info);
#endif

#if USE_OLD_MECHANISM_OF_HANDLING_PREPROCESSING_INFO
          void getDirectives ( char* sourceFilename );
#endif

       // DQ (1/30/2009): Added support for Fortran allocate and deallocate statements.
          void unparseAllocateStatement(SgStatement* stmt, SgUnparse_Info& info); 
          void unparseDeallocateStatement(SgStatement* stmt, SgUnparse_Info& info);

       // FMZ (2/17/200(): Added support for co-array fortran "withteam" stmt
          void unparseWithTeamStatement(SgStatement* stmt, SgUnparse_Info& info); 

      //! begin the unparser (unparser.C)
          void run_unparser();

      //FMZ (2/10/2009): Added support for unparsing SgCoExpresson
          void unparseCoArrayExpression      (SgExpression* expr, SgUnparse_Info& info);
void curprint(const std::string&) const;

};

#endif



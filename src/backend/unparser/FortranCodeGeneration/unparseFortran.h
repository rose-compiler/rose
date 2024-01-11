/* unparser.h
 * This header file contains the class declaration for the newest unparser. Six
 * C files include this header file: unparser.C, modified_sage.C, unparse_stmt.C, 
 * unparse_expr.C, unparse_type.C, and unparse_sym.C.
 */

#ifndef UNPARSE_FORTRAN_H
#define UNPARSE_FORTRAN_H

#include "unparser.h"

class FortranCodeGeneration_locatedNode : public UnparseLanguageIndependentConstructs
   {
     public:
          FortranCodeGeneration_locatedNode(Unparser* unp, std::string fname);
          virtual ~FortranCodeGeneration_locatedNode();

      //! Support for Fortran include mechanism (not a Fortran statement in the grammar)
          virtual void unparseFortranIncludeLine ( SgStatement* stmt, SgUnparse_Info& info );

      //! Support for Fortran numeric labels (can appear on any statement), this is an empty function for C/C++.
          virtual void unparseStatementNumbers(SgStatement* stmt, SgUnparse_Info& info) override;

      //! Supporting function for unparseStatementNumbers and for direct use in unparsing statments that have an end_numeric_label field.
          void unparseStatementNumbersSupport ( SgLabelRefExp* numeric_label_exp, SgUnparse_Info& info );

          void unparse_helper (SgFunctionDeclaration* funcdecl_stmt, SgUnparse_Info& info);

          void unparseFunctionArgs(SgFunctionDeclaration* funcdecl_stmt, SgUnparse_Info& info);
          void unparseFunctionParameterDeclaration ( SgFunctionDeclaration* funcdecl_stmt, 
                                                     SgInitializedName* initializedName,
                                                     bool outputParameterDeclaration, 
                                                     SgUnparse_Info& info );

          virtual void unparseLanguageSpecificStatement (SgStatement*  stmt, SgUnparse_Info& info) override;
          virtual void unparseLanguageSpecificExpression(SgExpression* expr, SgUnparse_Info& info) override;

          virtual std::string languageName() const override {
            return "Fortran Unparser";
          }

          virtual void unparseUnaryOperator           (SgExpression* expr, const char* op, SgUnparse_Info& info);
          virtual void unparseBinaryOperator          (SgExpression* expr, const char* op, SgUnparse_Info& info);
          virtual void unparseExprRoot                (SgExpression* expr, SgUnparse_Info& info);

          virtual void unparseExprList                (SgExpression* expr, SgUnparse_Info& info) override;
          virtual void unparseVarRef                  (SgExpression* expr, SgUnparse_Info& info);
          virtual void unparseClassRef                (SgExpression* expr, SgUnparse_Info& info);
          virtual void unparseFuncRef                 (SgExpression* expr, SgUnparse_Info& info);
          virtual void unparseMFuncRef                (SgExpression* expr, SgUnparse_Info& info);

          virtual void unparseStringVal               (SgExpression* expr, SgUnparse_Info& info) override;
          virtual void unparseBoolVal                 (SgExpression* expr, SgUnparse_Info& info) override;
          virtual void unparseFuncCall                (SgExpression* expr, SgUnparse_Info& info);
          virtual void unparsePointStOp               (SgExpression* expr, SgUnparse_Info& info);
          virtual void unparseRecRef                  (SgExpression* expr, SgUnparse_Info& info);
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
          virtual void unparseLShiftOp                (SgExpression* expr, SgUnparse_Info& info);
          virtual void unparseRShiftOp                (SgExpression* expr, SgUnparse_Info& info);
          virtual void unparseUnaryMinusOp            (SgExpression* expr, SgUnparse_Info& info);
          virtual void unparseUnaryAddOp              (SgExpression* expr, SgUnparse_Info& info);
          virtual void unparseNotOp                   (SgExpression* expr, SgUnparse_Info& info);
          virtual void unparseDerefOp                 (SgExpression* expr, SgUnparse_Info& info);
          virtual void unparseAddrOp                  (SgExpression* expr, SgUnparse_Info& info);
          virtual void unparseBitCompOp               (SgExpression* expr, SgUnparse_Info& info);
          virtual void unparseCastOp                  (SgExpression* expr, SgUnparse_Info& info);
          virtual void unparseArrayOp                 (SgExpression* expr, SgUnparse_Info& info);
          virtual void unparseNewOp                   (SgExpression* expr, SgUnparse_Info& info);
          virtual void unparseDeleteOp                (SgExpression* expr, SgUnparse_Info& info);
          virtual void unparseAssnOp                  (SgExpression* expr, SgUnparse_Info& info);
          virtual void unparsePointerAssnOp           (SgExpression* expr, SgUnparse_Info& info);
          virtual void unparseTypeRef                 (SgExpression* expr, SgUnparse_Info& info);
          virtual void unparseInitializerList         (SgExpression* expr, SgUnparse_Info& info);
          virtual void unparseAggrInit                (SgExpression* expr, SgUnparse_Info& info);
          virtual void unparseConInit                 (SgExpression* expr, SgUnparse_Info& info);
          virtual void unparseAssnInit                (SgExpression* expr, SgUnparse_Info& info);

       // Fortran specific member functions
          virtual void unparseExpOp                    (SgExpression* expr, SgUnparse_Info& info);
          virtual void unparseSubscriptExpr            (SgExpression* expr, SgUnparse_Info& info);
          virtual void unparseColonShapeExp            (SgExpression* expr, SgUnparse_Info& info);
          virtual void unparseAsteriskShapeExp         (SgExpression* expr, SgUnparse_Info& info);

          virtual void unparseIOItemExpr               (SgExpression* expr, SgUnparse_Info& info);
          virtual void unparseImpliedDo                (SgExpression* expr, SgUnparse_Info& info);
          virtual void unparseConcatenationOp          (SgExpression* expr, SgUnparse_Info& info);

          virtual void unparseUnknownArrayOrFunctionReference (SgExpression* expr, SgUnparse_Info& info);

          virtual bool isSubroutineCall                (SgFunctionCallExp* fcall);

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
          virtual void unparseProcessControlStmt       (SgStatement* stmt, SgUnparse_Info& info);

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

          virtual void unparseSyncAllStatement         (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseSyncImagesStatement      (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseSyncMemoryStatement      (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseSyncTeamStatement        (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseLockStatement            (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseUnlockStatement          (SgStatement* stmt, SgUnparse_Info& info);

          virtual void unparse_Image_Ctrl_Stmt_Support (SgImageControlStatement* stmt, bool print_comma, SgUnparse_Info& info);
          virtual void unparseAssociateStatement       (SgStatement* stmt, SgUnparse_Info& info);

       // Unparse base class data members
          virtual bool unparse_IO_Support              (SgStatement* stmt, bool skipUnit, SgUnparse_Info& info);
          virtual void unparse_IO_Control_Support      ( std::string name, SgExpression* expr, bool isLeadingEntry, SgUnparse_Info& info);

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
          virtual void unparseBasicBlockStmt   (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseIfStmt           (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseFuncDefnStmt     (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseVarDeclStmt      (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseVarDefnStmt      (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseClassDeclStmt_module      (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseClassDeclStmt_derivedType (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseClassDefnStmt    (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseExprStmt         (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseLabelStmt        (SgLabelStatement* stmt, SgUnparse_Info& info);
          virtual void unparseWhileStmt        (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseSwitchStmt       (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseCaseStmt         (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseDefaultStmt      (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseBreakStmt        (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseContinueStmt     (SgContinueStmt*,   SgUnparse_Info& info);
          virtual void unparseFortranContinueStmt (SgFortranContinueStmt*, SgUnparse_Info& info);
          virtual void unparseReturnStmt       (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseGotoStmt         (SgGotoStatement*,  SgUnparse_Info& info);

          virtual void unparseAttributeSpecificationStatement (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseNamelistStatement(SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseImportStatement  (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseBindAttribute    (SgDeclarationStatement* declaration );

          virtual void unparseFormatStatement  (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseFormatItemList   (SgFormatItemList* formatItemList, SgUnparse_Info& info);
          virtual void unparseForAllStatement  (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseDoConcurrentStatement (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseContainsStatement(SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseEntryStatement   (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparsePragmaDeclStmt   (SgStatement* stmt, SgUnparse_Info& info);

       // Supporting function shared by unparseClassDecl and unparseClassType
          void initializeDeclarationsFromParent ( SgDeclarationStatement* declarationStatement,
                                                  SgClassDefinition* & cdefn,
                                                  SgNamespaceDefinitionStatement* & namespaceDefn, int debugSupport = 0 );

       // kind and type parameters.
          void unparseModifierType(SgType* type, SgUnparse_Info& info);

       // labels (to support arithmetic if)
          void unparseLabel ( SgLabelRefExp* exp );
          virtual void unparseLabelRefExp(SgExpression* expr, SgUnparse_Info& info);

          virtual void unparseActualArgumentExpression(SgExpression* expr, SgUnparse_Info& info);

          void unparseAllocateStatement(SgStatement* stmt, SgUnparse_Info& info); 
          void unparseDeallocateStatement(SgStatement* stmt, SgUnparse_Info& info);

      //! begin the unparser (unparser.C)
          void run_unparser();

       // unparsing for Rice Coarray Fortran 2.0
          void unparseWithTeamStatement(SgStatement* stmt, SgUnparse_Info& info); 
          void unparseCoArrayExpression(SgExpression* expr, SgUnparse_Info& info);
          void curprint(const std::string&) const;

       // unparsing for OpenMP AST
          virtual void unparseOmpPrefix          (SgUnparse_Info& info) override;
          virtual void unparseOmpDoStatement     (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseOmpBeginDirectiveClauses    (SgStatement* stmt, SgUnparse_Info& info) override;
          virtual void unparseOmpEndDirectiveClauses      (SgStatement* stmt, SgUnparse_Info& info) override;
          virtual void unparseOmpEndDirectivePrefixAndName(SgStatement* stmt, SgUnparse_Info& info) override;

         /**
          * override to make unary plus/minus expressions have the same precedence as binary plus;
          * everything else remains the same.
          */
          virtual bool requiresParentheses(SgExpression* expr, SgUnparse_Info& info) override;
          virtual PrecedenceSpecifier getPrecedence(SgExpression* exp) override;

     private:
          /**
          * For unparsing the dimension attribute of an entity in a type declaration.
          * @param oneVarOnly - true means there is only one declared variable in the type declaration.
          * If oneVarOnly is false , unparses dimension attribute of the given array type and other
          * attributes of its base type.
          */
          void unparseArrayAttr(SgArrayType* type, SgUnparse_Info& info, bool oneVarOnly);

          /**
          * For unparsing the length attribute of an entity in a type declaration.
          * @param oneVarOnly - true means there is only one declared variable in the type declaration.
          * If oneVarOnly is false , unparses length attribute of the given string type
          */
          void unparseStringAttr(SgTypeString* type, SgUnparse_Info& info, bool oneVarOnly);

          /**
          * For unparsing the attributes of an entity in a type declaration.
          * @param oneVarOnly - true means there is only one declared variable in the type declaration.
          * If oneVarOnly is false , unparses relevant attributes of the given type.
          */
          void unparseEntityTypeAttr(SgType* type, SgUnparse_Info& info, bool oneVarOnly);

          /**
          * For unparsing language keywords (allows option for upper or lower case)
          * @param keyword - the keyword to unparse
          * @param info - object containing unparsing options
          */
          void curprint_keyword(const std::string &keyword, SgUnparse_Info& info);
};

#endif // UNPARSE_FORTRAN_H
